/***************************************************************************
 *            sample.cpp
 *
 *  Copyright  2006 - 2013 Florent Berthaut, 2019 Jean-Emmanuel Doucet & Aurélien Roux
 *  florentberthaut@no-log.org jean-emmanuel.doucet@groolot.net orl@ammd.net
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "sample.h"

using namespace std;

sample::sample():name("no sample yet"),buffers(NULL),loaded(false),absoluteName(""),relativeName(""),nbUsers(0)
{
}

sample::~sample()
{
	unload();
}

jack_default_audio_sample_t** sample::getBuffers()
{
	if(loaded)
		return buffers;
	else
		return NULL;
}

int sample::tryLoad(char* file,char* relativeFile,std::string tapFile)
{
	int res=-1;
	if(!load(file))
	{
		processFileName(std::string(file),tapFile);
		loaded=true;
		res=0;
	}
	else
	{
		if(relativeFile!=NULL)
		{
			size_t found = tapFile.find_last_of("/\\");
			std::string relativeToTapFile;
			if(found!=tapFile.npos)
			{
				relativeToTapFile=tapFile.substr(0,found)+"/"+relativeFile;
			}
			else
			{
				relativeToTapFile=relativeFile;
			}
			size_t size = relativeToTapFile.size() + 1;
			char * buffer = new char[ size ];
			strncpy( buffer, relativeToTapFile.c_str(), size );
			if(!load(buffer))
			{
				processFileName(std::string(relativeFile),tapFile);
				res=0;
				loaded=true;
			}
			delete [] buffer;
		}
	}
	if(res)
	{
		name="No Sample Yet";
		absoluteName="";
		relativeName="";
	}

	return res;
}


int sample::load(char* file)
{
	//get the file
	SF_INFO sfInfo;
	sfInfo.format=0;
	SNDFILE* sndFile = sf_open(file,SFM_READ,&sfInfo);

	if(!sndFile)
		return -1;

	//test the samplerate
	sampleRate = sfInfo.samplerate;

	//channels
	channelsCount =  sfInfo.channels;

	//frames number
	framesCount = (long)(sfInfo.frames);

	//create the buffers
	buffers = new jack_default_audio_sample_t*[channelsCount];
	for(int i=0;i<channelsCount;++i) {
		buffers[i] = new jack_default_audio_sample_t[framesCount];

	}


/*
	float cf[channelsCount];
	//read the files into the buffers
	for(int i=0;i<framesCount;++i) {
		sf_readf_float(sndFile,cf,1);
		for(int j=0;j<channelsCount;++j)
		{
			buffers[j][i]=(jack_default_audio_sample_t)cf[j];
		}
	}
*/

	int maxNbFramesToRead=10000;
	float cf[channelsCount * maxNbFramesToRead];
	int nbFrames;
	int pos = 0;
	while ((nbFrames = sf_readf_float(sndFile,cf,maxNbFramesToRead)) > 0) {
		for (int i = 0; i < nbFrames*channelsCount; ++pos) {
			for(int j=0;j<channelsCount;++j) {
				buffers[j][pos] = (jack_default_audio_sample_t)cf[i];
				++i;
			}
		}
	}


	//close the file
	if(sf_close(sndFile))
		return -1;
	return 0;
}

void sample::setSampleRate(int sr)
{
	//if samplerate different from jack's samplerate -> resample
	if(sr != sampleRate && loaded)
	{
		loaded=false;
		long newFramesCount = (long)((double)framesCount / (double) sampleRate * (double)sr);

		jack_default_audio_sample_t** convs = new jack_default_audio_sample_t*[channelsCount];
		SRC_DATA *datas = new SRC_DATA[channelsCount];

		for(int i=0;i<channelsCount;++i)
		{
			convs[i] = new jack_default_audio_sample_t[newFramesCount];

			datas[i].data_in = buffers[i];
			datas[i].data_out = convs[i];
			datas[i].input_frames = framesCount;
			datas[i].output_frames = newFramesCount;
			datas[i].src_ratio = (double)sr / (double)sampleRate;

			src_simple(datas+i, 0, 1) ;
		}

		for(int i=0;i<channelsCount;++i)
		{
			delete [] buffers[i];
			buffers[i] = new jack_default_audio_sample_t[datas[i].output_frames_gen];

			for(int j=0;j<datas[i].output_frames_gen;++j)
			{
				buffers[i][j] = convs[i][j];
			}
		}

		framesCount = newFramesCount;
		sampleRate = sr;

		for(int i=0;i<channelsCount;++i)
		{
			delete [] convs[i];
		}
		delete [] convs;
		delete [] datas;

		loaded=true;
	}
}

void sample::unload()
{
	//unload the file
	if(buffers)
	{
		for(int i=0;i<channelsCount;++i)
			delete [] buffers[i];
		delete [] buffers;
	}
	loaded=false;
}


std::string sample::getName()
{
	return name;
}

std::string sample::getAbsoluteName()
{
	return absoluteName;
}

std::string sample::getRelativeName()
{
	return relativeName;
}

void sample::addUser()
{
	++nbUsers;
}

int sample::getNbUsers()
{
	return nbUsers;
}

void sample::removeUser()
{
	--nbUsers;
}

void sample::getFrame(const int& channel, const double& offset, jack_default_audio_sample_t& frame)
{
	if(loaded)
	{
		//FIXME interpolation thanks pete bessman
		long off1,off2,off3,off4;
		off1 = ((long)offset - 1 < 0 )?0:(long)offset-1;
		off2 = (long)offset;
		off3 = ((long)offset + 1 >= framesCount)?0:(long)offset+1;
		off4 = ((long)offset + 2 >= framesCount)?0:(long)offset+2;

		//short d = ((long)offset>>24);
		//cout<<"offset "<<offset<<endl;
		//cout<<"calc "<<(offset - (double)((long)offset))<<endl;
		//unsigned char d = (int)((offset - floor(offset)) * (0xFFFFFFFFU)) % 256;
		unsigned char d = (int)((offset - floor(offset)) * (0xFFFFFFFFU)) >> 24;
		//cout<<d<<endl;
		if(channelsCount==1) { //mono sound
			frame = buffers[0][off1]*ct0[d] + buffers[0][off2]*ct1[d] + buffers[0][off3]*ct2[d] + buffers[0][off4]*ct3[d];
		}
		else {			//stereo sound
			frame = buffers[channel][off1]*ct0[d] + buffers[channel][off2]*ct1[d] + buffers[channel][off3]*ct2[d] + buffers[channel][off4]*ct3[d];
		}
	}
	else
	{
		frame=0;
	}




/*
    if ((y0 = (v->posi - 1) * 2) < 0)
	y0 = 0;
    y1 = v->posi * 2;
    if ((y2 = (v->posi + 1) * 2) >= p->sample->frames * 2)
	y2 = 0;
    if ((y3 = (v->posi + 2) * 2) >= p->sample->frames * 2)
	y3 = 0;

    *l = cerp (p->sample->sp[y0],
	       p->sample->sp[y1],
	       p->sample->sp[y2], p->sample->sp[y3], v->posf >> 24);
    *r = cerp (p->sample->sp[y0 + 1],
	       p->sample->sp[y1 + 1],
	       p->sample->sp[y2 + 1], p->sample->sp[y3 + 1], v->posf >> 24);
float cerp(float y0, float y1, float y2, float y3, guint8 d)
{
    float s0;

    s0 = y0 * ct0[d];
    s0 += y1 * ct1[d];
    s0 += y2 * ct2[d];
    s0 += y3 * ct3[d];

    return s0;
}
*/

/*
	if(loaded)
	{
		if(channelsCount==1) //mono sound
			return buffers[0][(long)offset];
		else			//stereo sound
			return buffers[channel][(long)offset];
	}
	else
			return 0;

*/
}


void sample::processFileName(std::string f,std::string t)
{
	std::string absolutePath="";
	std::string pathFromTap="";

	//GET WORKING DIRECTORY
	char * buffer = new char[ 1024 ];
	std::string currentDirectory(getcwd(buffer, 1024));
	delete [] buffer;

	//GET TAP FILE ABSOLUTE PATH
	if(t[0]!='/') //get the absolute path of the tap file
	{
		if(t==".") //if path like .
			t = "";

		//remove the ../ if any
		std::string tmpCD = currentDirectory;
		bool bback=false;
		while(!bback)
		{
			size_t back = t.find("../");
			size_t back2 = tmpCD.find_last_of("/\\");
			if(back==t.npos)
				bback=true;
			else
			{
				t = t.substr(back+3);
				tmpCD = tmpCD.substr(0,back2);
			}
		}
		if(t!="")
		{
			t = tmpCD+"/"+t;
		}
		else
		{
			t = tmpCD;
		}
	}
	//get last '/'
	size_t found = t.find_last_of("/\\");

	//NAME OF THE FILE
	name= t.substr(found+1);

	//ABSOLUTE PATH OF THE TAP
	std::string absoluteTapPath = t.substr(0,found);


	//GET ABSOLUTE SAMPLE PATH
	if(f[0]!='/') //relative path -> get the absolute path from tap path
	{
		if(f[0]=='.' && f[1]=='/') //if path like ./
			f = f.substr(2);

		//remove the ../ if any
		std::string tmpCD = absoluteTapPath;
		bool bback=false;
		while(!bback)
		{
			size_t back = f.find("../");
			size_t back2 = tmpCD.find_last_of("/\\");
			if(back==f.npos)
				bback=true;
			else
			{
				f = f.substr(back+3);
				tmpCD = tmpCD.substr(0,back2);
			}
		}
		f = tmpCD+"/"+f;
	}

	//get last '/'
	found = f.find_last_of("/\\");

	//NAME OF THE FILE
	name= f.substr(found+1);

	//ABSOLUTE PATH OF THE FILE
	absolutePath = f.substr(0,found);

	bool root=false;
	std::string tmpF = absolutePath+"/";
	std::string tmpT = absoluteTapPath+"/";
	//get the root of the two paths
	while(!root)
	{
		size_t slashF = tmpF.find_first_of("/\\")+1;
		size_t slashT = tmpT.find_first_of("/\\")+1;

		std::string subF = tmpF.substr(0,slashF);
		std::string subT = tmpT.substr(0,slashT);

		if(subF!=subT || (subF.length()==0 && subT.length()==0)) // different path
		{
			root=true;
		}
		else
		{
			tmpF = tmpF.substr(slashF);
			tmpT = tmpT.substr(slashT);
		}
	}

	//get the number of directories in the tap path
	root=false;
	while(tmpT.length()>0)
	{
		size_t slashT = tmpT.find_first_of("/\\")+1;
		tmpT = tmpT.substr(slashT);
		pathFromTap+="../";
	}

	//get path from tap
	pathFromTap = pathFromTap+tmpF;

	//so we have the absolute name
	absoluteName = absolutePath+"/"+name;

	//if same directory, use ./
	if(pathFromTap.length()==0)
		pathFromTap="./";

	//and we have the relative name
	relativeName = pathFromTap+name;
}
