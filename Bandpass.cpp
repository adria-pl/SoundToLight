// =======================================================
// Bandpass.cpp
// Implementació en cascada del filtre passabanda
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

#include "Bandpass.h"
#include <math.h>
// this implementation lives here in order not to require math.h in the public header

template <typename T>
void BiquadCoeffT<T>::calc()
{
	sample_t theta = 2*M_PI*Fc;
	sample_t K = tan(M_PI * Fc);
	sample_t sinTheta = sin(theta);
	sample_t cosTheta = cos(theta);
	sample_t alphaT = sinTheta/(2*Q);
	a0 = alphaT / b0;
	a1 = 0 / b0;
	a2 = (-a0) / b0;
	b0 = 1+alphaT;
	b1 = (-2*cosTheta) / b0;
	b2 = (1-alphaT) / b0;
	return;
}

template class BiquadCoeffT<float>;
