#include "synth_additive.h"

namespace resample {

/// @brief A pointer to a signal buffer and its length.
struct buffer {
    float *t;
    int len;
};

/// @brief Sample function definition.
using sample_func = float (*)(int, buffer);


float sample_oneshot(int i, buffer wave) {
    if (i < 0 || i >= wave.len) {
        return 0;
    }

    return wave.t[i];
}

float sample_loop(int k, buffer wave) {
    if (k < 0) {
        int i = 5;
        while (k < 0) {
            k += wave.len;
            if (i-- < 0) {
                Serial.print("whuuhh?");
            }
        }
    }
    else if (k >= wave.len) {
        if (wave.len == 256) {
            k = k & 0xff;
        }
        else {
            k = k % wave.len;
        }
    }

    return wave.t[k];
}

float sinc(float x) {
    if (abs(x) <= std::numeric_limits<float>::epsilon()) return 1;
    return arm_sin_f32(x * PI) / (x * PI);
}

std::array<float, 576> windowTableData {
	-1.3877787807814457e-17, 0.0664466094067262, 0.3399999999999999, 0.7735533905932737, 0.9999999999999999, 0.7735533905932739, 0.3400000000000001, 0.06644660940672628, -1.3877787807814457e-17,
	0, 0.06409788563992366, 0.3338883253614437, 0.7672248264226534, 0.9999382564152686, 0.7798287109237423, 0.3461598636471639, 0.06884857701368072, 1.3554576124064077e-05,
	0, 0.061801872470459936, 0.32782574924213004, 0.7608448584223401, 0.9997530458445159, 0.786048955421927, 0.3523669777650424, 0.07130431368527311, 5.4227148311658535e-05,
	0, 0.059558029251766974, 0.3218131519950253, 0.754415333809753, 0.99944442882847, 0.7922123009241798, 0.35862037493638443, 0.07381433601430017, 0.00012204424012048076,
	0, 0.057365808548885296, 0.31585138470251517, 0.7479381061971443, 0.999012506236362, 0.7983169339038444, 0.3649190590299332, 0.07637915135012596, 0.00021705003118953348,
	0, 0.0552246567542076, 0.30994126908099884, 0.7414150347791486, 0.9984574192179714, 0.8043610512776669, 0.3712620053832077, 0.07899925718897689, 0.0003393063178221828,
	0, 0.05313401470318664, 0.30408359740298374, 0.7348479835204592, 0.9977793491365277, 0.8103428612096714, 0.3776481610026512, 0.08167514056668268, 0.0004888924578373699,
	0, 0.05109331828959461, 0.29827913243666476, 0.7282388203440715, 0.9969785174825043, 0.8162605839120536, 0.3840764447811048, 0.08440727745428024, 0.0006659052997262799,
	0, 0.04910199907992176, 0.29252860740296116, 0.7215894163205324, 0.9960551857683568, 0.8221124524426588, 0.39054574773252176, 0.08719613215688694, 0.000870459096159959,
	0, 0.04715948492650232, 0.2868327259499761, 0.7149016448586435, 0.9950096554042603, 0.827896713498597, 0.3970549332438594, 0.09004215671625729, 0.0011026854019042381,
	0, 0.045265200578957936, 0.2811921621448283, 0.7081773808980522, 0.9938422675549184, 0.8336116282055642, 0.40360283734404445, 0.09294579031742539, 0.0013627329562085344,
	0, 0.04341856829355008, 0.27560756048280166, 0.7014185001041708, 0.9925534029775251, 0.8392554729024337, 0.410188268989928, 0.09590745869984542, 0.0016507675497451635,
	0, 0.0416190084400345, 0.27007953591374234, 0.6946268780658597, 0.9911434818409672, 0.8448265399206795, 0.4168100103691044, 0.09892757357342645, 0.0019669718761862187,
	0, 0.03986594010561392, 0.26460867388562637, 0.6878043894963082, 0.9896129635263722, 0.8503231383582086, 0.4234668172194878, 0.10200653203986927, 0.0023115453685137455,
	0, 0.03815878169558573, 0.25919553040520765, 0.6809529074375451, 0.9879623464091123, 0.8557435948471696, 0.43015741916550915, 0.10514471601969985, 0.0026847040201710554,
	0, 0.0364969515302864, 0.25384063211565033, 0.6740743024690076, 0.9861921676223873, 0.8610862543153117, 0.43688052007079137, 0.10834249168539437, 0.0030866801911709485,
	0, 0.03487986843793456, 0.2485444763910329, 0.6671704419205938, 0.984303002802518, 0.8663494807404799, 0.44363479840716147, 0.11160020890099182, 0.0035177223992877427,
	0, 0.03330695234298013, 0.24330753144760825, 0.6602431890906238, 0.9822954658160962, 0.8715316578978184, 0.4504189076398267, 0.11491820066857758, 0.003978095096468612,
	0, 0.031777624849569, 0.2381302364716896, 0.653294402469126, 0.9801702084691396, 0.8766311900992763, 0.45723147662855956, 0.1182967825820289, 0.0044680784306112,
	0, 0.030291309819735913, 0.23301300176402312, 0.6463259349668657, 0.9779279201984212, 0.8816465029250012, 0.46407111004469426, 0.12173625228839693, 0.0049879679928611365,
	0, 0.028847433945943753, 0.2279562089004996, 0.639339633150531, 0.9755693277451404, 0.8865760439462159, 0.47093638880376365, 0.12523688895730936, 0.005538074550596378,
	0, 0.02744542731758931, 0.22296021090904578, 0.632337336484476, 0.9730951948111229, 0.8914182834391762, 0.47782587051356024, 0.12879895275875838, 0.006118723766270817,
	0, 0.02608472398110199, 0.21802533246252903, 0.6253208765794342, 0.9705063216977416, 0.8961717150898135, 0.4847380899374275, 0.1324226843496503, 0.006730255902301821,
	0, 0.024764762493264467, 0.21315187008749686, 0.6182920764485936, 0.9678035449277593, 0.900834856688671, 0.49167155947255026, 0.13610830436947102, 0.007373025512193668,
	0, 0.023484986467390646, 0.20834009238856516, 0.6112527497714307, 0.964987736850308, 0.9054062508157457, 0.4986247696430276, 0.13985601294543298, 0.008047401118099179,
	0, 0.022244845112000644, 0.2035902402882592, 0.6042047001656924, 0.9620598052292236, 0.9098844655148546, 0.5055961896074876, 0.14366598920745252, 0.00875376487502981,
	0, 0.021043793761637, 0.19890252728210264, 0.5971497204679086, 0.95902069281497, 0.9142680949571523, 0.5125842676809942, 0.147538390813302, 0.00949251222193323,
	0, 0.019881294399473226, 0.19427713970874003, 0.5900895920228135, 0.955871376900389, 0.9185557600934272, 0.5195874318710032, 0.15147335348428614, 0.010264051519867853,
	0, 0.018756816171369955, 0.18971423703487098, 0.5830260839820492, 0.9526128688605292, 0.9227461092948134, 0.526604090427091, 0.15547099055176727, 0.011068803677508544,
	0, 0.017669835891040937, 0.18521395215476397, 0.5759609526125163, 0.9492462136768062, 0.9268378189815634, 0.5336326324041987, 0.15953139251487936, 0.011907201764231289,
	0, 0.016619838535996106, 0.18077639170410914, 0.5688959406147334, 0.9457724894457661, 0.9308295942395269, 0.5406714282390972, 0.16365462660974353, 0.012779690611027246,
	0, 0.015606317733936455, 0.17640163638796386, 0.5618327764515586, 0.9421928068727251, 0.9347201694239943, 0.5477188303398015, 0.16784073639051067, 0.013686726399509574,
	0, 0.014628776239280425, 0.17208974132253127, 0.554773173687621, 0.9385083087505672, 0.9385083087505672, 0.5547731736876215, 0.17208974132253121, 0.014628776239280453,
	0, 0.013686726399509554, 0.1678407363905106, 0.5477188303398014, 0.9347201694239942, 0.9421928068727251, 0.5618327764515588, 0.1764016363879639, 0.015606317733936483,
	0, 0.012779690611027239, 0.1636546266097436, 0.5406714282390973, 0.9308295942395268, 0.9457724894457662, 0.5688959406147337, 0.1807763917041094, 0.016619838535996155,
	0, 0.011907201764231275, 0.1595313925148792, 0.5336326324041983, 0.9268378189815633, 0.9492462136768063, 0.5759609526125165, 0.18521395215476394, 0.01766983589104093,
	0, 0.011068803677508544, 0.15547099055176725, 0.526604090427091, 0.9227461092948133, 0.9526128688605293, 0.5830260839820496, 0.18971423703487114, 0.018756816171370004,
	0, 0.010264051519867853, 0.15147335348428598, 0.519587431871003, 0.9185557600934271, 0.9558713769003891, 0.5900895920228136, 0.19427713970874003, 0.019881294399473226,
	0, 0.009492512221933244, 0.147538390813302, 0.5125842676809941, 0.9142680949571522, 0.95902069281497, 0.5971497204679089, 0.1989025272821028, 0.02104379376163705,
	0, 0.008753764875029754, 0.14366598920745235, 0.5055961896074873, 0.9098844655148546, 0.9620598052292237, 0.6042047001656923, 0.20359024028825926, 0.02224484511200065,
	0, 0.008047401118099193, 0.13985601294543293, 0.49862476964302743, 0.9054062508157456, 0.9649877368503081, 0.6112527497714307, 0.20834009238856532, 0.023484986467390702,
	0, 0.007373025512193668, 0.1361083043694708, 0.49167155947254987, 0.9008348566886709, 0.9678035449277596, 0.6182920764485934, 0.21315187008749675, 0.024764762493264425,
	0, 0.006730255902301835, 0.1324226843496502, 0.4847380899374274, 0.8961717150898134, 0.9705063216977416, 0.6253208765794344, 0.2180253324625291, 0.026084723981102016,
	0, 0.006118723766270873, 0.12879895275875847, 0.47782587051356035, 0.8914182834391761, 0.973095194811123, 0.6323373364844763, 0.22296021090904605, 0.027445427317589435,
	0, 0.005538074550596406, 0.12523688895730933, 0.47093638880376354, 0.8865760439462158, 0.9755693277451404, 0.6393396331505311, 0.22795620890049972, 0.028847433945943773,
	0, 0.004987967992861123, 0.12173625228839702, 0.46407111004469437, 0.8816465029250011, 0.9779279201984213, 0.6463259349668662, 0.23301300176402345, 0.030291309819736004,
	0, 0.004468078430611172, 0.1182967825820288, 0.45723147662855934, 0.876631190099276, 0.9801702084691397, 0.6532944024691261, 0.23813023647168965, 0.03177762484956904,
	0, 0.003978095096468612, 0.11491820066857758, 0.4504189076398267, 0.8715316578978183, 0.9822954658160963, 0.6602431890906242, 0.24330753144760847, 0.03330695234298023,
	0, 0.003517722399287715, 0.11160020890099166, 0.4436347984071612, 0.8663494807404798, 0.9843030028025181, 0.6671704419205938, 0.2485444763910329, 0.03487986843793455,
	0, 0.0030866801911709624, 0.10834249168539431, 0.43688052007079126, 0.8610862543153115, 0.9861921676223873, 0.6740743024690077, 0.25384063211565056, 0.03649695153028644,
	0, 0.0026847040201710415, 0.10514471601969971, 0.43015741916550887, 0.8557435948471693, 0.9879623464091124, 0.680952907437545, 0.25919553040520765, 0.038158781695585745,
	0, 0.002311545368513704, 0.10200653203986924, 0.42346681721948765, 0.8503231383582086, 0.9896129635263722, 0.6878043894963082, 0.2646086738856265, 0.03986594010561392,
	0, 0.001966971876186191, 0.09892757357342627, 0.41681001036910403, 0.8448265399206794, 0.9911434818409672, 0.6946268780658595, 0.27007953591374234, 0.041619008440034466,
	0, 0.0016507675497451635, 0.09590745869984535, 0.41018826898992783, 0.8392554729024335, 0.9925534029775251, 0.701418500104171, 0.27560756048280177, 0.04341856829355015,
	0, 0.0013627329562085205, 0.09294579031742546, 0.4036028373440445, 0.8336116282055642, 0.9938422675549186, 0.7081773808980526, 0.2811921621448287, 0.045265200578958054,
	0, 0.0011026854019042381, 0.09004215671625718, 0.39705493324385926, 0.8278967134985968, 0.9950096554042603, 0.7149016448586436, 0.2868327259499763, 0.047159484926502356,
	0, 0.000870459096159959, 0.08719613215688699, 0.3905457477325219, 0.8221124524426587, 0.9960551857683569, 0.7215894163205329, 0.2925286074029615, 0.049101999079921826,
	0, 0.0006659052997262799, 0.08440727745428013, 0.3840764447811046, 0.8162605839120536, 0.9969785174825043, 0.7282388203440716, 0.29827913243666476, 0.05109331828959465,
	0, 0.0004888924578373699, 0.08167514056668268, 0.3776481610026512, 0.8103428612096713, 0.9977793491365277, 0.7348479835204595, 0.30408359740298396, 0.05313401470318674,
	0, 0.00033930631782219667, 0.0789992571889768, 0.37126200538320747, 0.8043610512776668, 0.9984574192179714, 0.7414150347791487, 0.3099412690809989, 0.055224656754207596,
	0, 0.00021705003118953348, 0.07637915135012596, 0.3649190590299332, 0.7983169339038443, 0.999012506236362, 0.7479381061971446, 0.3158513847025154, 0.05736580854888533,
	0, 0.00012204424012042525, 0.07381433601430003, 0.3586203749363842, 0.7922123009241796, 0.99944442882847, 0.7544153338097533, 0.3218131519950253, 0.05955802925176693,
	0, 5.4227148311658535e-05, 0.07130431368527307, 0.3523669777650423, 0.7860489554219268, 0.9997530458445159, 0.7608448584223401, 0.32782574924213015, 0.06180187247046004,
	0, 1.3554576124077955e-05, 0.06884857701368055, 0.34615986364716356, 0.7798287109237423, 0.9999382564152686, 0.7672248264226535, 0.3338883253614436, 0.0640978856399236,
};
arm_bilinear_interp_instance_f32 windowTableInterpolator { 64, 9, windowTableData.data() };


float window(float m, int M, bool debugFlag) {
    // there's an interpolation-based speedup here, but it's not huge. It needs a 2D table, which winds up being a lot of
    // divisions to bring everything into scale and then do the interpolations. The fast trig functions here only
    // eat about 3% of the DSP budget, and I'm not ready to optimize it yet.

    if (m >= 0 && m <= M) {
        return 0.42f - (0.5f * arm_cos_f32((2 * PI * m) / M)) + (0.08f * arm_cos_f32((4 * PI * m) / M));
    }

    return 0;
}

float fast_window(float windowOffset, int ki) {
    return arm_bilinear_interp_f32(&windowTableInterpolator, ki, windowOffset * (windowTableInterpolator.numRows - 1));
}

float mod(float x, int m) {
    float intPart;
    float fracPart = modff(x, &intPart);
    return ((int) intPart % m) + fracPart;
}

float windowed_sinc_interpolation(buffer input, buffer output, float inputSampleRate, float outputSampleRate, sample_func samplePolicy, float phase, bool debugFlag) {
    const int windowSize = 8;
    const int halfWindow = windowSize / 2;

    const float sincScale = min(inputSampleRate, outputSampleRate) / inputSampleRate;
    const float sampleRatio = inputSampleRate / outputSampleRate;

    for (int j = 0; j < output.len; j++) {
        float J = j * sampleRatio + phase;
        int kLow = (int) ceilf(J - halfWindow);

        float accum = 0;
        for (int ki = 0; ki <= windowSize; ki++) {
            float sampleOffset = kLow + ki - J;
            auto winScale = window(sampleOffset + halfWindow, windowSize, debugFlag);
            accum +=  sinc(sincScale * sampleOffset) * winScale * samplePolicy(kLow + ki, input);
        }
        output.t[j] = min(1.f, outputSampleRate / inputSampleRate) * accum;
    }

    return mod(mod((output.len) * sampleRatio, input.len) + phase, input.len);
}


float pitch_shift_looped(buffer loop, buffer stream, float nativeSampleRate, float originalPitch, float targetPitch, float phase, bool debugFlag) {
    float shiftedRate = nativeSampleRate * (originalPitch / targetPitch);
    return windowed_sinc_interpolation(loop, stream, nativeSampleRate, shiftedRate, sample_loop, phase, debugFlag);
}

float pitch_shift_single_cycle(buffer loop, buffer stream, float nativeSampleRate, float targetPitch, float phase, float debugFlag) {
    float originalPitch = nativeSampleRate / loop.len;
    return pitch_shift_looped(loop, stream, nativeSampleRate, originalPitch, targetPitch, phase, debugFlag);
}


} //namespace resample

void AudioSynthAdditive::update() {
    auto block = allocate();
    if (!block) return;

    // super naive additive algorithm follows for now, lol

    decltype(partialTable) workingArray;
    memcpy(workingArray.data(), partialTable.data(), sizeof(float) * partialTable.size());

    // calculate the waveform for this frame
    arm_rfft_fast_f32(&fftInstance, workingArray.data(), signal.data(), 1);

    playbackPhase = 
        resample::pitch_shift_single_cycle({signal.data(), signal_table_size}, {workingArray.data(), AUDIO_BLOCK_SAMPLES}, AUDIO_SAMPLE_RATE_EXACT, sampler.frequency, playbackPhase, useWindow);

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
        block->data[i] = workingArray.data()[i] * 32000;
    }

    // for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    //     block->data[i] = sampler.sample() * 32000;
    //     sampler.step();
    // }

    transmit(block);
    release(block);
}