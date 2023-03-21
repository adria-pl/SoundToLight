// =======================================================
// Render.cpp
//
//
//  █████╗ ██████╗ ██████╗ ██╗ █████╗     ██████╗ ██╗     
// ██╔══██╗██╔══██╗██╔══██╗██║██╔══██╗    ██╔══██╗██║     
// ███████║██║  ██║██████╔╝██║███████║    ██████╔╝██║     
// ██╔══██║██║  ██║██╔══██╗██║██╔══██║    ██╔═══╝ ██║     
// ██║  ██║██████╔╝██║  ██║██║██║  ██║    ██║     ███████╗
// ╚═╝  ╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝    ╚═╝     ╚══════╝
//
// Copyright 2023 Adrià Piqueras Lluelles
// =======================================================

#include <Bela.h>
#include <cmath>
#include <libraries/Scope/Scope.h>
#include <libraries/UdpClient/UdpClient.h>
#include "Bandpass.h"

Scope scope;

// UDP
AuxiliaryTask gUdpTask;
UdpClient* udp_client;
const char* IPAddr = "192.168.1.1";
int Port = NULL;

Biquad bpFilter1;	// Primer filtre biquadràtic
Biquad bpFilter2;	// Segon filtre biquadràtic
Biquad bpFilter3;	// Tercer filtre biquadràtic
Biquad bpFilter4;	// Quart filtre biquadràtic

int bpFreq1 = 0, bpFreq2 = 1, bpFreq3 = 2, bpFreq4 = 3;   // Lectura potenciòmetres freqüència
int dGate1 = 4, dGate2 = 5, dGate3 = 6, dGate4 = 7;   // Lectura potenciòmetres llindar porta dinàmica

float bpGate1,bpGate2,bpGate3,bpGate4;

float gFilterQ = 0.707; // Factor de qualitat dels filtres biquadràtics

int anUpdate = 10;  // Freqüència de refresc dels potenciòmetres
int updateFreq = 0; // Inicialització
int canalsPLC = 4;  // Número de canals a enviar al PLC

/*
void writeBuffer(void*) {
  unsigned int numItems = anUpdate * canalsPLC;
  float buf[numItems];
  udp_client->send((void*)buf, sizeof(buf));
}
*/

bool setup(BelaContext *context, void *userData)
{
    updateFreq = context->analogFrames / anUpdate;
    
    scope.setup(9,22050);

	// udp_client = new UdpClient(Port, IPAddr);

	return true;
}


void render(BelaContext *context, void *userData)
{
    Biquad::Settings settings{
			.fs = context->audioSampleRate,
			.cutoff = 30,
			.q = gFilterQ,
			};
    
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float monoSum = 0.0; // Mono sum of input channels
		float out1, out2, out3, out4 = 0.0;

        // Llegir valor potenciòmetres
        if(updateFreq && !(n % updateFreq)) {
        	/*
            settings.cutoff = map(analogRead(context, n/updateFreq, bpFreq1), 0, 1, 30, 15000);
            bpFilter1.setup(settings);
            
            settings.cutoff = map(analogRead(context, n/updateFreq, bpFreq2), 0, 1, 30, 15000);
            bpFilter2.setup(settings);
            settings.cutoff = map(analogRead(context, n/updateFreq, bpFreq3), 0, 1, 30, 15000);
            bpFilter3.setup(settings);
            settings.cutoff = map(analogRead(context, n/updateFreq, bpFreq4), 0, 1, 30, 15000);
            bpFilter4.setup(settings);
            
            //bpGate1 = analogRead(context, n/updateFreq, dGate1);
            //bpGate2 = map(analogRead(context, n/updateFreq, dGate2), 0, 1, 100, 1000);
            bpGate3 = map(analogRead(context, n/updateFreq, dGate3), 0, 1, 100, 1000);
            bpGate4 = map(analogRead(context, n/updateFreq, dGate4), 0, 1, 100, 1000);
            */
        }

		// Sum all input channels to mono
		for(unsigned int ch = 0; ch < context->audioInChannels; ch++)
			monoSum += audioRead(context, n, ch);
		// Divide sum by the number of channels
		monoSum = monoSum / context->audioInChannels;

		/*
		
		*/
		if(!(n % 10)) {
		settings.cutoff = map(analogRead(context, n, bpFreq1), 0, 1, 30, 15000);
		bpFilter1.setup(settings);
		settings.cutoff = map(analogRead(context, n, bpFreq2), 0, 1, 30, 15000);
        bpFilter2.setup(settings);
        settings.cutoff = map(analogRead(context, n, bpFreq3), 0, 1, 30, 15000);
        bpFilter3.setup(settings);
        settings.cutoff = map(analogRead(context, n, bpFreq4), 0, 1, 30, 15000);
        bpFilter4.setup(settings);
		bpGate1 = analogRead(context, n, dGate1);
		bpGate2 = analogRead(context, n, dGate2);
		bpGate3 = analogRead(context, n, dGate3);
		bpGate4 = analogRead(context, n, dGate4);
		}
		
		// Process input signal with filters
		out1 = bpFilter1.process(monoSum);
		out2 = bpFilter2.process(monoSum);
		out3 = bpFilter3.process(monoSum);
		out4 = bpFilter4.process(monoSum);
		
		scope.log(monoSum,out1,out2,out3,out4,bpGate1,bpGate2,bpGate3,bpGate4);
		
		audioWrite(context, n, 0, out1);
        audioWrite(context, n, 1, out2);

		// std::vector<std::vector<float> out[4] = {out1}; //, out2, out3, out4};

		// udp_client->send(out, sizeof(out));

/*
		for(unsigned int ch = 0; ch < context->audioOutChannels; ch++)
			audioWrite(context, n, ch, out1);
*/
	}

	// Iniciar tasca auxiliar UDP
    // Bela_scheduleAuxiliaryTask(gUdpTask);
}

void cleanup(BelaContext *context, void *userData)
{
	//delete udp_client;
}
