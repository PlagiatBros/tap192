#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <complex.h>
#include <fftw3.h>
#include "gnuplot_i.h"

#define SAMPLING_RATE 44100.0
#define CHANNELS_NUMBER 1
#define NB_BANDES 10

#define real double

static int N=2048;

typedef real frame[2048];
typedef complex spectrum[2048];
static gnuplot_ctrl *temporal = NULL;




/* input */

static char *SOUND_FILE_NAME_READ = "tmp-in.raw";

static real cache_in[2048];

static fftw_plan plan = NULL;
static fftw_plan plan_inv = NULL;
static fftw_complex* in =NULL;
static fftw_complex *out =NULL;
static fftw_complex* in_inv =NULL;
static fftw_complex *out_inv =NULL;



void fft_init()
{
	
  	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*N);
 	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*N);

      	plan = fftw_plan_dft_1d(N,in,out,FFTW_FORWARD, FFTW_ESTIMATE);

}

void i_fft_init()
{
	
  	in_inv = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*N);
 	out_inv = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*N);


      	plan_inv = fftw_plan_dft_1d(N,in_inv,out_inv,FFTW_BACKWARD, FFTW_ESTIMATE);

}

void  fft(frame s)
{
	//on transforme le tableau de doubles en tab de fftw_complex
	int i;
	for(i=0;i<N;++i)
		in[i]=s[i] + I*0.0;

	//on execute la transfo
	fftw_execute(plan);
	
}

void  i_fft(double *amp,double *phs)
{
	//on transforme les tableaux de doubles en un tab de fftw_complex
	int i;
	for(i=0;i<N;++i)
		in_inv[i]=amp[i]*cos(phs[i]) + I*(amp[i]*sin(phs[i]));

	//on execute la transfo
	fftw_execute(plan_inv);
}


void fft_exit()
{

	fftw_destroy_plan(plan);
	free(out);
	free(in);
	fftw_destroy_plan(plan_inv);
	free(out_inv);
	free(in_inv);

}

void cartesian_to_polar (complex *S,double *amp,double *phs,int n)
{
	int i;
	for(i=0;i<n;++i)
	{
		amp[i]=(double)cabs(S[i]);
		phs[i]=(double)carg(S[i]);
	}
	
}


void mul_fen_hann(frame s)
{
	double hann[N];
	int i;
	for(i=0;i<N;++i)
		hann[i]=0.5*(1-cos(2*3.14*i/N));
	for(i=0;i<N;++i)
		s[i]=s[i]*hann[i];
}


void fen_zero_phase(frame s)
{
	int i;
	for(i=0;i<N/2;++i)
	{
		float tmp = s[i];
	        s[i]=s[i+N/2];
		s[i+N/2]=tmp;	
	}
}

void robotisation(double *phs)
{
	int i;
	for(i=0;i<N;++i)
		phs[i]=0.0;
}



//calcul du cepstre 
void calcul_cepstre(double *amp,double *phs,double *cepstre)
{
	double *tmp = (double*) malloc(N*sizeof(double));
		
	
	int i;
	for(i=0;i<N;++i)
	{
		tmp[i] = log(amp[i]);
	}


	//transformée inverse

	//on transforme les tableaux de doubles en un tab de fftw_complex
	for(i=0;i<N;++i)
		in_inv[i]=(fftw_complex)tmp[i];

	//on execute la transfo
	fftw_execute(plan_inv);

	//le resultat est dans out_inv
  	//recupere le signal
  	int l; 
	for(l=0;l<N;++l)
		cepstre[l]=creal(out_inv[l]);
	
	free(tmp);
}

//fenetre de filtrage passe bas du cepstre
void calcul_fen_hpb(double* fen,int ordre)
{
	int i;
	fen[0]=1;
	for(i=1;i<ordre/2;++i)
		fen[i]=2;
	fen[ordre/2]=1;
	for(i=ordre/2+1;i<N;++i)
		fen[i]=0;
}




//calcul_enveloppe_spectrale
void calcul_env_spectr(double *env,double *amp,double* phs )
{
	int ordre = 50;
	double *cepstre = (double*) malloc(N*sizeof(double));
	double *fen = (double*) malloc(N*sizeof(double));
	
		
	//calcul du cesptre
	calcul_cepstre(amp,phs,cepstre);
	//calcul de la fenetre de filtrage passe bas
	calcul_fen_hpb(fen,ordre);

	int i;
	

	//filtrage du cepstre
	for(i=0;i<N;++i)
		cepstre[i]*=fen[i];

	//calcul de l'enveloppe spectrale
	
		//transformee de fourier
		fft(cepstre);
			
		//on recupere le resultat dans out	
		int l;
		for(l=0;l<N;++l)
			env[l]=cabs(cexp(out[l]/N));


		

}


void decale_env_spect(double *env,int decalage)
{
	int i;

	//on applique le decalage
	if(decalage<0)
	{
		decalage*=(-1);
		for(i=decalage;i<N/2;++i)
		{
			env[i-decalage]=env[i];
		}
		for(i=N/2-decalage;i<N/2;++i)
		{
			env[i]=0;
		}

	}
	else
	{
		for(i=N/2;i>=decalage;--i)
		{
			env[i]=env[i-decalage];
		}
		for(i=decalage;i>=0;--i)
		{
			env[i]=0;
		}
	}

	//symetrique
	for(i=0;i<N/2;++i)
	{
		env[N-1-i]=env[i];
	}
}


void passe_bas(double* env, int f)
{
	int i;
	for(i=f;i<N/2;i++)
		env[i]=0;
	//symetrique
	for(i=0;i<N/2;++i)
	{
		env[N-1-i]=env[i];
	}
	
}

void passe_haut(double* env, int f)
{
	int i;
	for(i=0;i<f;i++)
		env[i]=0;
	//symetrique
	for(i=0;i<N/2;++i)
	{
		env[N-1-i]=env[i];
	}
	
}




FILE *
sound_file_open_read (char *sound_file_name)
{
#define MAX_CMD 256
  char cmd[MAX_CMD];

  assert (sound_file_name);

  snprintf (cmd, MAX_CMD,
	    "sox %s -c %d -r %d -sw %s",
	    sound_file_name,
	    CHANNELS_NUMBER, (int)SAMPLING_RATE, SOUND_FILE_NAME_READ);

  system (cmd);

  bzero (cache_in, N/2*sizeof(real));

  return fopen (SOUND_FILE_NAME_READ, "rb");
}

void
sound_file_close_read (FILE *fp)
{
  assert (fp);

  fclose (fp);

  
  unlink (SOUND_FILE_NAME_READ);
}

int
sound_file_read (FILE *fp, frame s)
{
  int result;

  int i;

  static short tmp[2048/2*CHANNELS_NUMBER];

  assert (fp && s);


  //on remplit la premiere moitie de la fenetre avec la partie memorisée
  
  for (i=0; i<N/2; i++)
    {
      s[i] = cache_in[i];
    }

  bzero (tmp, N/2*CHANNELS_NUMBER*sizeof(short));


  //on remplit la deuxieme moitie de la fenetre avec la premiere moitie de la nouvelle frame
  result = fread (tmp, sizeof(short), N/2*CHANNELS_NUMBER, fp);

  for (i=0; i<N/2; i++)
    {
      cache_in[i] = s[N/2+i] = tmp[i*CHANNELS_NUMBER] / 32768.0;
    }

  return (result == N/2);
}

/* output */

static char *SOUND_FILE_NAME_WRITE = "tmp-out.raw";
static char *sound_file_name_write = "out.wav";

static real cache_out[2048];

FILE *
sound_file_open_write (void)
{
  bzero (cache_out, N/2*sizeof(real));

  return fopen (SOUND_FILE_NAME_WRITE, "wb");
}

void
sound_file_close_write (FILE *fp)
{
#define MAX_CMD 256
  char cmd[MAX_CMD];

  assert (fp);

  fclose (fp);

  snprintf (cmd, MAX_CMD,
	    "sox -c %d -r %d -sw %s %s",
	    CHANNELS_NUMBER, (int)SAMPLING_RATE, SOUND_FILE_NAME_WRITE,
	    sound_file_name_write);

  system (cmd);

  unlink (SOUND_FILE_NAME_WRITE);
}

void
sound_file_write (frame s, FILE *fp)
{
  int i;

  static short tmp[2048/2*CHANNELS_NUMBER];

  assert (fp && s);

  for (i=0; i<N/2; i++)
    {
	   //on va ajouter la fin de la trame précédente a la trame courante 
      real v = s[i] + cache_out[i];
      short _s = (v < -1) ? -32768 : (v > 1) ? 32767 : (short) (v * 32767);

      //puis memoriser la deuxieme partie de la trame courante
      cache_out[i] = s[N/2+i];

      int channel;

      for (channel=0; channel<CHANNELS_NUMBER; channel++)
	{
	  tmp[i*CHANNELS_NUMBER+channel] = _s;
	}
    }

  fwrite (tmp, sizeof(short), N/2*CHANNELS_NUMBER, fp);
}


int
main (int argc, char *argv[])
{


  /* handle gnuplot */
  temporal = gnuplot_init ();
  gnuplot_setstyle (temporal, "lines");
  gnuplot_resetplot (temporal);
  struct timespec a;


	
  FILE *input, *output;

  /* temporal */
  frame s;

  assert (sizeof(short) == 2);

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s <soundfile>\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  /* init */
  //plot_init ();
  fft_init();
  i_fft_init();
  double *amp = (double*) malloc(N*sizeof(double));
  double *phs = (double*) malloc(N*sizeof(double));
  double *env = (double*) malloc(N*sizeof(double));
  double *source = (double*) malloc(N*sizeof(double));

  input = sound_file_open_read (argv[1]);
  output = sound_file_open_write ();


  
  
  /* process */
  while (sound_file_read (input, s))
    {
	    
	//on applique deja une fenetre de hann pour ameliorer l'analyse
	mul_fen_hann(s);

	//on applique le fenetrage zero phase
	fen_zero_phase(s);
	
      	// Fourier analysis 
	fft(s);
	
	//le resultat est dans out
	//on recupere les spectres d'amplitude et de phase
	cartesian_to_polar(out,amp,phs,N);		

	//Calcul de l'enveloppe spectrale(filtre) et de la source
	calcul_env_spectr(env,amp,phs);	
	
	int i;
	for(i =0;i<N;++i)
		source[i] = amp[i]/env[i];
	
	
	//Decalage de l'enveloppe spectrale

	//decale_env_spect(env,-50);	

	//Passe 
	passe_haut(env,300);

	//PitchShifing : on bidouille la source puis on reapplique l'enveloppe
	// spectrale
	

	//on recalcule le signal
	for(i =0;i<N;++i)
		amp[i]= env[i]*source[i];
	
	//Affichage du spectre et de l'enveloppe
	/* nanosecondes*/
	a.tv_nsec = (long)100000000;
	/* secondes */
	a.tv_sec=(time_t)0;
	/*gnuplot_resetplot (temporal);
	gnuplot_plot_x(temporal,env, N, "enveloppe");
	gnuplot_plot_x(temporal,amp, N, "spectre d'amplitude");
	*/
	
  	//sleep(1); 
	
      	// Fourier synthesis 

	i_fft(amp,phs);

	//le resultat est dans out_inv
  	//recupere le signal
  	int l; 
	for(l=0;l<N;++l)
		s[l]=creal(out_inv[l])/(N);

	//on applique le fenetrage zero phase
	fen_zero_phase(s);
	
	//on ecrit la frame dans le fichier de sortie	
	
	sound_file_write (s, output);
    }

  
  /* exit */

  sound_file_close_write (output);
  sound_file_close_read (input);

  fft_exit();
  /* close handle gnuplot */
  gnuplot_close(temporal);

  free(amp);
  free(phs);
  free(env);

  
  exit (EXIT_SUCCESS);

  return 0;
}
