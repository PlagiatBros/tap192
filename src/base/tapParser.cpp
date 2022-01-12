// This file is part of tap192
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "../package.h"
#include "tapParser.h"
#include "tapeutape.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
using namespace std;

tapParser::tapParser(tapeutape *t):tap(t)
{
    LIBXML_TEST_VERSION
    doc=NULL;
    root_element=NULL;
    optionsMode = false;
    creationMode = true;
}

tapParser::~tapParser()
{
    xmlCleanupParser();
}

int tapParser::createTree(char* f)
{
    fileName=f;
    doc= xmlReadFile(f,NULL,0);
    root_element = xmlDocGetRootElement(doc);
    if (doc == NULL || root_element==NULL){
        std::string fn(f);
        tap->showMessage(true,"Failed to parse "+fn);
        return -1;
    }

    if (xmlStrcmp(root_element->name, (const xmlChar *) XML_NAMESPACE) && xmlStrcmp(root_element->name, (const xmlChar *) XML_OLDNAMESPACE)) {
        tap->showMessage(true,"Document of the wrong type, root node != (tap192 || tapeutape)");
        xmlFreeDoc(doc);
        return -1;
    }

    return 0;
}

int tapParser::parseTree()
{
    int res=0;
    char* val=NULL;

    //we start with the first node
    xmlNodePtr a_node = root_element->children;

    //get tapeutape file version
    val=(char*)xmlGetProp(root_element,(xmlChar*)"version");
    if (val==NULL) {
        if (parse005(a_node))
            res = -1;
        else
            res = saveToFile(fileName);
    }
    else
        res = parseCurrent(a_node);

    xmlFreeDoc(doc);
    return res;
}

int tapParser::parseCurrent(xmlNodePtr a_node)
{
    xmlNodePtr cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        std::string nodeName((const char*)(cur_node->name));
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (nodeName=="options") {
                if (parseOptions(cur_node))
                    return -1;
            }
            else if (nodeName=="creation") {
                if (parseCreation(cur_node))
                    return -1;
            }
        }
    }
    return 0;
}

int tapParser::parseOptions(xmlNodePtr cur_node)
{
    char* val=NULL;

    //gets the global volume
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"globalvolume");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : globalvolume missing in the options node");
        return -1;
    }
    tap->setGlobalVolume(atof(val));
    val=NULL;

    //sets the max polyphony
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"max_polyphony");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : max_polyphony missing in the options node");
        return -1;
    }
    tap->setPolyphony(atoi(val));

    xmlNodePtr cNode = NULL;

    for (cNode = cur_node->children; cNode; cNode = cNode->next) {
        val = NULL;
        std::string nodeName((const char*)(cNode->name));
        if (cNode->type == XML_ELEMENT_NODE) {
            if (nodeName=="jack-output") {
                val=(char*)xmlGetProp(cNode,(xmlChar*)"name");
                if (val==NULL) {
                    tap->showMessage(true,"Problem in the .tap file : name missing in a jack-output node");
                    return -1;
                }
                tap->addJackStereoChannel(val);
            }
        }
    }

    return 0;
}

int tapParser::parseCreation(xmlNodePtr cur_node)
{
    xmlNodePtr cNode = NULL;
    for (cNode = cur_node->children; cNode; cNode = cNode->next) {
        std::string nodeName((const char*)(cNode->name));
        if (cNode->type == XML_ELEMENT_NODE) {
            if (nodeName=="setup")
                if (parseSetup(cNode))
                    return -1;
        }
    }
    return 0;
}

int tapParser::parseSetup(xmlNodePtr cur_node)
{
    char* val=NULL;

    //create a new setup
    setup* s = new setup();
    tap->addSetup(s);

    unsigned short chan,cc ;
    val =(char*)xmlGetProp(cur_node,(xmlChar*)"channel");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : channel missing in a setup node");
        return -1;
    }
    chan = (unsigned short)atoi(val);
    val=NULL;

    val = (char*)xmlGetProp(cur_node,(xmlChar*)"cc");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : cc missing in a setup node");
        return -1;
    }
    cc = (unsigned short)atoi(val);
    s->setCC(chan,cc);
    val=NULL;

    val= (char*)xmlGetProp(cur_node,(xmlChar*)"name");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : name missing in a setup node");
        return -1;
    }
    std::string name(val);
    s->setName(name);

    xmlNodePtr cNode = NULL;
    for (cNode = cur_node->children; cNode; cNode = cNode->next) {
        val = NULL;
        std::string nodeName((const char*)(cNode->name));
        if (cNode->type == XML_ELEMENT_NODE) {
            if (nodeName=="kit")
                if (parseKit(cNode,s))
                    return -1;
        }
    }

    return 0;
}

int tapParser::parseKit(xmlNodePtr cur_node,setup* s)
{
    char* val=NULL;

    //add a new kit
    kit* k = new kit();
    s->addKit(k);

    //set its name
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"name");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : name missing in a kit node");
        return -1;
    }
    k->setName(val);

    //set its volume
    val=NULL;
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"volume");
    if (val!=NULL) {
        k->setVolume(atof(val));
    }

    xmlNodePtr cNode = NULL;
    for (cNode = cur_node->children; cNode; cNode = cNode->next) {
        val = NULL;
        std::string nodeName((const char*)(cNode->name));
        if (cNode->type == XML_ELEMENT_NODE) {
            if (nodeName=="instrument")
                if (parseInstrument(cNode,k))
                    return -1;
        }
    }

    return 0;
}

int tapParser::parseInstrument(xmlNodePtr cur_node,kit* k)
{
    char* val=NULL;

    //add a new instrument to the current kit
    instrument* ins = new instrument();
    k->addInstrument(ins);

    //set its name
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"name");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : name missing in an instrument node");
        return -1;
    }
    ins->setName(val);
    val=NULL;

    //sets its minimum note
    int prop ;
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"minnote");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : minnote missing in an instrument node");
        return -1;
    }
    ins->setMinNote(atoi(val));
    val=NULL;

    //sets its maximum note
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"maxnote");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : maxnote missing in an instrument node");
        return -1;
    }
    ins->setMaxNote(atoi(val));
    val=NULL;

    //sets its root note
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"rootnote");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : rootnote missing in an instrument node");
        return -1;
    }
    ins->setRootNote(atoi(val));
    val=NULL;

    //sets its root note fine
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"rootnotefine");
    float fine=0;
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : rootnotefine missing in an instrument node");
    }
    else {
        fine = atof(val);
    }
    ins->setRootNoteFine(fine);
    val=NULL;

    //sets its channel
    val=(char*)xmlGetProp(cur_node,(xmlChar*)"channel");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : channel missing in an instrument node");
        return -1;
    }
    ins->setMidiChannel(atoi(val));
    val=NULL;

    //sets its polyphony
    val =  (char*)xmlGetProp(cur_node,(xmlChar*)"polyphony");
    if (val!=NULL)
        ins->setPolyphony(atoi(val));
    else
        ins->setPolyphony(0);

    val=NULL;

    //sets its cut
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"cut");
    if (val!=NULL)
        ins->setCut(atoi(val));
    else
        ins->setCut(-1);
    val=NULL;

    //sets its jack channel
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"output");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : output missing in an instrument node");
        return -1;
    }
    prop= tap->getJackStereoChannel(val);
    if (prop==-1) {
        tap->showMessage(true,"Problem in the .tap file : unknown output in an instrument node");
        prop=0;
    }
    ins->setJackStereoChannel(prop);
    val=NULL;

    //sets its volume
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"volume");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : volume missing in an instrument node");
        return -1;
    }
    ins->setVolume(atof(val));
    val=NULL;

    //sets its pan
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"pan");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : pan missing in an instrument node");
        return -1;
    }
    ins->setPan(atof(val));

    //sets if we pitch over range
    char* por =  (char*)xmlGetProp(cur_node,(xmlChar*)"pitch_over_range");
    if (por!=NULL) {
        if (por[0]=='t')
            ins->setPitchOverRange(true);
        else
            ins->setPitchOverRange(false);
    }

    //sets the play mode
    char* pm =  (char*)xmlGetProp(cur_node,(xmlChar*)"play_mode");
    if (pm!=NULL) {
        if (pm[0]=='n')
            ins->setPlayMode(0);
        else if (pm[0]=='t')
            ins->setPlayMode(1);
    }

    pm=NULL;
    //sets the play loop
    pm =  (char*)xmlGetProp(cur_node,(xmlChar*)"play_loop");
    if (pm!=NULL) {
        if (pm[0]=='y')
            ins->setPlayLoop(1);
        else if (pm[0]=='n')
            ins->setPlayLoop(0);
    }

    pm=NULL;
    //sets the play reverse
    pm =  (char*)xmlGetProp(cur_node,(xmlChar*)"play_reverse");
    if (pm!=NULL) {
        if (pm[0]=='y')
            ins->setPlayReverse(1);
        else if (pm[0]=='n')
            ins->setPlayReverse(0);
    }

    //get the variations
    xmlNodePtr cNode = NULL;
    for (cNode = cur_node->children; cNode; cNode = cNode->next) {
        val = NULL;
        std::string nodeName((const char*)(cNode->name));
        if (cNode->type == XML_ELEMENT_NODE) {
            if (nodeName=="variation")
                if (parseVariation(cNode,ins))
                    return -1;
        }
    }

    return 0;

}

int tapParser::parseVariation(xmlNodePtr cur_node,instrument* ins)
{
    char* val=NULL;
    //add a new variation to the current instrument
    variation* var = new variation();
    ins->addVariation(var);

    //sets its minimum velocity
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"minveloc");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : minveloc missing in a variation node");
        return -1;
    }
    var->setMinVeloc(atoi(val));
    val=NULL;

    //sets its maximum velocity
    val = (char*)xmlGetProp(cur_node,(xmlChar*)"maxveloc");
    if (val==NULL) {
        tap->showMessage(true,"Problem in the .tap file : maxveloc missing in a variation node");
        return -1;
    }
    var->setMaxVeloc(atoi(val));

    char* n=NULL;
    char* rn = NULL;
    //gets its sample
    n = (char*)xmlGetProp(cur_node,(xmlChar*)"sample");
    rn = (char*)xmlGetProp(cur_node,(xmlChar*)"sampleRelative");
    if (n==NULL) {
        tap->showMessage(true,"Problem in the .tap file : sample missing in a variation node");
        return -1;
    }

    var->setSample(tap->addSample(n,rn));

    usleep(100);

    return 0;
}

int tapParser::saveToFile(char* filename)
{

    char buf[32];

    xmlDocPtr doc = NULL;
    xmlNodePtr rootNode = NULL, node1 = NULL, node2 = NULL,node3 = NULL,node4 = NULL,node5 = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");
    rootNode = xmlNewNode(NULL, BAD_CAST XML_NAMESPACE);
    xmlNewProp(rootNode, BAD_CAST "version", BAD_CAST VERSION );
    xmlDocSetRootElement(doc, rootNode);

    //option node
    node1 = xmlNewChild(rootNode, NULL, BAD_CAST "options",NULL);
    sprintf(buf, "%f", tap->getGlobalVolume());
    xmlNewProp(node1, BAD_CAST "globalvolume", BAD_CAST buf );
    sprintf(buf, "%d", tap->getPolyphony());
    xmlNewProp(node1, BAD_CAST "max_polyphony", BAD_CAST buf);

                                 //jackChannels nodes
    for(int i=0;i<tap->getNbJackStereoChannels();++i) {
        node2=xmlNewChild(node1,NULL,BAD_CAST "jack-output",NULL);
        size_t size = tap->getJackStereoChannelName(i).size() + 1;
        char * buffer = new char[ size ];
        strncpy(buffer, tap->getJackStereoChannelName(i).c_str(), size );
        xmlNewProp(node2, BAD_CAST "name", BAD_CAST buffer);
        delete [] buffer;
    }

    //creation node
    node1 = xmlNewChild(rootNode, NULL, BAD_CAST "creation",NULL);
                                 //setup nodes
    for(int i=0;i<tap->getNbSetups();++i) {
        currentSetup=tap->getSetup(i);
        node2=xmlNewChild(node1,NULL,BAD_CAST "setup",NULL);
        xmlNewProp(node2, BAD_CAST "name", BAD_CAST currentSetup->getName().c_str());
        sprintf(buf, "%d", currentSetup->getChannel());
        xmlNewProp(node2, BAD_CAST "channel", BAD_CAST buf);
        sprintf(buf, "%d", currentSetup->getCC());
        xmlNewProp(node2, BAD_CAST "cc", BAD_CAST buf);
                                 //kit nodes
        for(int j=0;j<currentSetup->getNbKits();++j) {
            currentKit = currentSetup->getKit(j);
            node3=xmlNewChild(node2,NULL,BAD_CAST "kit",NULL);
            xmlNewProp(node3, BAD_CAST "name", BAD_CAST currentKit->getName().c_str());
            sprintf(buf, "%f", currentKit->getVolume());
            xmlNewProp(node3, BAD_CAST "volume", BAD_CAST buf);
                                 //instrument nodes
            for(int j=0;j<currentKit->getNbInstruments();++j) {
                currentInstru = currentKit->getInstrument(j);
                node4=xmlNewChild(node3,NULL,BAD_CAST "instrument",NULL);
                xmlNewProp(node4, BAD_CAST "name", BAD_CAST currentInstru->getName().c_str());
                sprintf(buf, "%d", currentInstru->getMinNote());
                xmlNewProp(node4, BAD_CAST "minnote", BAD_CAST buf);
                sprintf(buf, "%d", currentInstru->getMaxNote());
                xmlNewProp(node4, BAD_CAST "maxnote", BAD_CAST buf);
                sprintf(buf, "%d", currentInstru->getRootNote());
                xmlNewProp(node4, BAD_CAST "rootnote", BAD_CAST buf);
                sprintf(buf, "%f", currentInstru->getRootNoteFine());
                xmlNewProp(node4, BAD_CAST "rootnotefine", BAD_CAST buf);
                sprintf(buf, "%d", currentInstru->getMidiChannel());
                xmlNewProp(node4, BAD_CAST "channel", BAD_CAST buf);
                sprintf(buf, "%d", currentInstru->getCut());
                xmlNewProp(node4, BAD_CAST "cut", BAD_CAST buf);
                sprintf(buf, "%d", currentInstru->getPolyphony());
                xmlNewProp(node4, BAD_CAST "polyphony", BAD_CAST buf);
                size_t size = tap->getJackStereoChannelName(currentInstru->getJackStereoChannel()).size() + 1;
                char * buffer = new char[ size ];
                strncpy(buffer, tap->getJackStereoChannelName(currentInstru->getJackStereoChannel()).c_str(), size );
                xmlNewProp(node4, BAD_CAST "output", BAD_CAST buffer);
                delete [] buffer;
                sprintf(buf, "%f", currentInstru->getVolume());
                xmlNewProp(node4, BAD_CAST "volume", BAD_CAST buf);
                sprintf(buf, "%f", currentInstru->getPan());
                xmlNewProp(node4, BAD_CAST "pan", BAD_CAST buf);
                if (currentInstru->getPitchOverRange())
                    xmlNewProp(node4, BAD_CAST "pitch_over_range", BAD_CAST "true");
                else
                    xmlNewProp(node4, BAD_CAST "pitch_over_range", BAD_CAST "false");

                if (currentInstru->getPlayMode()==0)
                    xmlNewProp(node4, BAD_CAST "play_mode", BAD_CAST "normal");
                else if (currentInstru->getPlayMode()==1)
                    xmlNewProp(node4, BAD_CAST "play_mode", BAD_CAST "trigger");
                if (currentInstru->getPlayLoop()==0)
                    xmlNewProp(node4, BAD_CAST "play_loop", BAD_CAST "no");
                else if (currentInstru->getPlayLoop()==1)
                    xmlNewProp(node4, BAD_CAST "play_loop", BAD_CAST "yes");
                if (currentInstru->getPlayReverse()==0)
                    xmlNewProp(node4, BAD_CAST "play_reverse", BAD_CAST "no");
                else if (currentInstru->getPlayReverse()==1)
                    xmlNewProp(node4, BAD_CAST "play_reverse", BAD_CAST "yes");

                                 //variation nodes
                for(int k=0;k<currentInstru->getNbVariations();++k) {
                    currentVar = currentInstru->getVariation(k);
                    node5=xmlNewChild(node4,NULL,BAD_CAST "variation",NULL);
                    sprintf(buf, "%d", currentVar->getMinVeloc());
                    xmlNewProp(node5, BAD_CAST "minveloc", BAD_CAST buf);
                    sprintf(buf, "%d", currentVar->getMaxVeloc());
                    xmlNewProp(node5, BAD_CAST "maxveloc", BAD_CAST buf);
                    if (currentVar->getSample()) {
                        size_t size = currentVar->getSample()->getAbsoluteName().size() + 1;
                        char * buffer = new char[ size ];
                        strncpy(buffer, currentVar->getSample()->getAbsoluteName().c_str(), size );
                        xmlNewProp(node5, BAD_CAST "sample", BAD_CAST buffer);
                        delete [] buffer;
                        size = currentVar->getSample()->getRelativeName().size() + 1;
                        buffer = new char[ size ];
                        strncpy(buffer, currentVar->getSample()->getRelativeName().c_str(), size );
                        xmlNewProp(node5, BAD_CAST "sampleRelative", BAD_CAST buffer);
                        delete [] buffer;
                    }
                    else {
                        xmlNewProp(node5, BAD_CAST "sample", BAD_CAST "");
                        xmlNewProp(node5, BAD_CAST "sampleRelative", BAD_CAST "");

                    }
                }
            }
        }
    }

    xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);

    xmlFreeDoc(doc);

    xmlCleanupParser();

    return 0;
}

int tapParser::parse005(xmlNodePtr a_node)
{
    xmlNodePtr cur_node;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        char* val = NULL;
        std::string nodeName((const char*)(cur_node->name));
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (nodeName=="options") {
                if (parseOptions(cur_node))
                    return -1;
            }
            else if (nodeName=="config") {
                //for each setup
                xmlNodePtr sNode;
                for (sNode = cur_node->children; sNode; sNode = sNode->next) {
                    val = NULL;
                    std::string sNodeName((const char*)(sNode->name));
                    if (sNode->type == XML_ELEMENT_NODE && sNodeName=="setup") {
                        //create a new setup
                        setup* s = new setup();
                        tap->addSetup(s);
                        unsigned short chan,cc ;
                        val =(char*)xmlGetProp(sNode,(xmlChar*)"channel");
                        if (val==NULL) {
                            tap->showMessage(true,"Problem in the .tap file : channel missing in a setup node");
                            return -1;
                        }
                        chan = (unsigned short)atoi(val);
                        val=NULL;

                        val = (char*)xmlGetProp(sNode,(xmlChar*)"cc");
                        if (val==NULL) {
                            tap->showMessage(true,"Problem in the .tap file : cc missing in a setup node");
                            return -1;
                        }
                        cc = (unsigned short)atoi(val);
                        s->setCC(chan,cc);
                        val=NULL;

                        val= (char*)xmlGetProp(sNode,(xmlChar*)"name");
                        if (val==NULL) {
                            tap->showMessage(true,"Problem in the .tap file : name missing in a setup node");
                            return -1;
                        }
                        std::string name(val);
                        s->setName(name);

                        //for each kit in the setup
                        xmlNodePtr kNode;
                        for (kNode = sNode->children; kNode; kNode = kNode->next) {
                            val = NULL;
                            std::string kNodeName((const char*)(kNode->name));
                            if (kNode->type == XML_ELEMENT_NODE && kNodeName=="kit") {

                                val = (char*)xmlGetProp(kNode,(xmlChar*)"name");
                                if (val==NULL) {
                                    tap->showMessage(true,"Problem in the .tap file : name missing in a kit node");
                                    return -1;
                                }
                                std::string confKitName(val);

                                //find the corresponding kit in the creation section
                                xmlNodePtr creaNode;
                                for (creaNode = a_node; creaNode; creaNode = creaNode->next) {
                                    std::string creaNodeName((const char*)(creaNode->name));
                                    if (creaNode->type == XML_ELEMENT_NODE && creaNodeName=="creation") {
                                        //get back a little
                                        xmlNodePtr creaKitNode;
                                        for (creaKitNode = creaNode->children; creaKitNode; creaKitNode = creaKitNode->next) {
                                            std::string creaKitNodeName((const char*)(creaKitNode->name));
                                            if (creaKitNode->type == XML_ELEMENT_NODE && creaKitNodeName=="kit") {
                                                val=NULL;
                                                val = (char*)xmlGetProp(creaKitNode,(xmlChar*)"name");
                                                if (val!=NULL) {

                                                    std::string creaKitName(val);
                                                    //if we have found the good kit
                                                    //then parse it and add it to the setup
                                                    if (creaKitName==confKitName)
                                                        parseKit(creaKitNode,s);

                                                }
                                            }
                                        }
                                        //move again
                                    }
                                }

                            }

                        }
                    }
                }

            }
        }
    }
    return 0;
}
