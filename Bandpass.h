// =======================================================
// Bandpass.h
// Capçalera pel filtre passabanda en cascada
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

#pragma once

class BiquadCoeff
{
public:
	struct Settings {
		float fs; ///< Freqüència de sampleig
		float cutoff; ///< Freqüència de tall en Hz
		float q; ///< Factor de Qualitat
	};
};

/**
 * Classe per computar els coeficients del filtre biquadràtic.
 */
template <typename T>
class BiquadCoeffT : public BiquadCoeff
{
public:
	typedef T sample_t;
	BiquadCoeffT() {}
	BiquadCoeffT(const Settings& settings) { setup(settings); }
	int setup(const Settings& settings)
	{
		Fs = settings.fs;
		Fc = settings.cutoff / Fs;
		Q = settings.q;
		calc();
		return 0;
	}

	void setQ(sample_t Q) {
		this->Q = Q;
		calc();
	}
	void setFc(sample_t Fc) {
		this->Fc = Fc/this->Fs;
		calc();
	}

	sample_t getQ() { return Q; }
	sample_t getFc() { return Fc; }

	sample_t a0, a1, a2, b0, b1, b2;
private:
	sample_t Fc, Q;
	sample_t Fs;
	void calc(void);
};

/**
 * Classe per processar els filtres biquadràtics.
 */
class Biquad : public BiquadCoeffT<float> {
public:
	Biquad() {};
	Biquad(const Settings& s) {
		setup(s);
	}
	/**
	 * Processar un sample d'entrada i retornar un sample de sortida.
	 */
	sample_t process(sample_t in)
	{
		sample_t out1 = in * a0 + z1;
		z1 = in * a1 + z2 - b1 * out1;
		z2 = in * a2 - b2 * out1;

		sample_t in2 = out1;
		sample_t out2 = in2 * a0 + z3;
		z3 = in2 * a1 + z4 - b1 * out2;
		z4 = in2 * a2 - b2 * out2;

		sample_t in3 = out2;
		sample_t output = in3 * a0 + z5;
		z5 = in3 * a1 + z6 - b1 * output;
		z6 = in3 * a2 - b2 * output;

		return output;
	}

	/**
	 * Resetejar l'estat intern del filter a 0;
	 */
	void clean()
	{
		z1 = z2 = z3 = z4 = z5 = z6 = 0.0;
	}
protected:
	sample_t z1 = 0;
	sample_t z2 = 0;
	sample_t z3 = 0;
	sample_t z4 = 0;
	sample_t z5 = 0;
	sample_t z6 = 0;
};
extern template class BiquadCoeffT<float>;