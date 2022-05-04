#include "LMF_IO.h"
//#include "afx.h"
#include "conio.h"


// do not change the following 2 lines. Even if your number of channels is different.
#define NUM_CHANNELS (80)
#define NUM_IONS (32)


//////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[], char* envp[])
//////////////////////////////////////////////////////////////////////////////////////////
{
	#ifdef _DEBUG
		printf("\n***********************\n    SLOW DEBUG VERSION USED\n***********************\n");
	#endif

	unsigned int	number_of_hits[NUM_CHANNELS];
	int				TDC[NUM_CHANNELS][NUM_IONS];


//  --------------------------------------------------------
//	read Header information of template LMF-file (ignore this part)
	LMF_IO * LMF = new LMF_IO(NUM_CHANNELS, NUM_IONS);
	char template_name[270];
	sprintf(template_name,"%s",argv[0]);
	while (strlen(template_name) > 0) {
		if (template_name[strlen(template_name)-1] == '\\' || template_name[strlen(template_name)-1] == '/') {
			break;
		}
		template_name[strlen(template_name)-1] = 0;
	}

	sprintf(template_name+strlen(template_name),"%s","Cobold2011R5-2_Header-Template_1TDC8HP.lmf");

	if (!LMF->OpenInputLMF(template_name)) {
		printf("error: could not open the template file\n%s\n",template_name);
		printf("It must be in the same folder as the exe.\n");
		return false;
	}
	LMF->CloseInputLMF();
//  end of read Header information ----------------------------

	FILE * in_file = 0;
	int result = 0;
	if (argc < 2)
		result = fopen_s(&in_file, "input_file.dat", "rt");
	else
		result = fopen_s(&in_file, argv[1], "rt");

	if (result || (!in_file)) {
		printf("input file not found.\n");
		LMF->CloseOutputLMF();
		if (LMF) delete LMF;
		return 0;
	}

//	Open and prepare output file
	LMF->number_of_channels_output = 16;
	LMF->max_number_of_hits_output = 10;
	LMF->frequency = 1.;
	LMF->tdcresolution_output				= 0.016; // can be ignored
	LMF->TDC8HP.GroupingEnable_p66_output	= 0;	 // can be ignored
	LMF->TDC8HP.GroupRangeStart_p69			= -100.; // can be ignored
	LMF->TDC8HP.GroupRangeEnd_p70			= 1000.; // can be ignored
	LMF->OpenOutputLMF("output.lmf");

	double dTimeStamp = 0.; // timestamp in seconds


//  Write Events into new LMF file:
	while (true) {
		memset(number_of_hits,   0,sizeof(number_of_hits)   );	// clear number_of_hits[] array
		memset(TDC,   0,sizeof(TDC)   );	// clear TDC[] array

		for (int channel = 0;channel < LMF->number_of_channels_output; channel++)
		{
			if (!fscanf_s(in_file,"%i",&number_of_hits[channel])) break;
			for (unsigned int hit = 0;hit < number_of_hits[channel]; hit++)
			{
				double time_in_ns = 0.;
				if (!fscanf_s(in_file, "%lf", &time_in_ns)) break;
				TDC[channel][hit] = int(time_in_ns / LMF->tdcresolution_output); // convert nanoseconds to TDC-bins
			}
		}
		
		if (feof(in_file)) break;

		dTimeStamp += 1.; // here you can set your own event timestamp (optionally)

		LMF->WriteTDCData(dTimeStamp, number_of_hits,&TDC[0][0]);
	}


	LMF->CloseOutputLMF();

	if (LMF) delete LMF;
	if (in_file) fclose(in_file);

	return 0;
}
