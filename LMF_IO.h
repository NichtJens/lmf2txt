#ifndef _LMF_IO_
#define _LMF_IO_


#include <fstream>
#include <ctime>
#include <cstring>


#ifndef LINUX
#ifndef WIN32
#ifndef WIN64
#if !(defined(__APPLE__) && defined(__MACH__))
#define LINUX
#endif
#endif
#endif
#endif


#if defined(LINUX) || defined(__APPLE__)
#define _fseeki64 fseeko
#define _ftelli64 ftello
#ifndef __int32_IS_DEFINED
#define __int32_IS_DEFINED
#define __int32 int
#define __int16 short
#define __int64 long long
#define __int8 char
#endif
#endif


#if defined(WIN32) || defined(WIN64)
#pragma warning(disable : 4996)
#endif


#define CRONO_OK 0
#define CRONO_WINDRIVER_NOT_FOUND 1
#define CRONO_DEVICE_NOT_FOUND 2
#define CRONO_NOT_INITIALIZED 3
/** No card with the matching board id was found in the device list*/
#define CRONO_ERROR_TRIGGER_CARD_NOT_FOUND 4
/* hen a capture on a closed card is called*/
#define CRONO_WRONG_STATE 4
/* The pointer given to a d125 function was not a valid pointer*/
#define CRONO_INVALID_DEVICE 5
#define CRONO_SYNC_NDIGO_MASTER_REQUIRED 6
#define CRONO_SYNC_INVALID_CONFIG 100


#define CRONO_PACKET_TYPE_16_BIT_SIGNED 1
#define CRONO_PACKET_TYPE_TIMESTAMP_ONLY 128
#define CRONO_PACKET_TYPE_TDC_RISING 144
#define CRONO_PACKET_TYPE_TDC_FALLING 145
#define CRONO_PACKET_TYPE_END_OF_BUFFER 129
#define CRONO_PACKET_TYPE_TDC_DATA    8
#define CRONO_READ_OK 0
#define CRONO_READ_NO_DATA 1
#define CRONO_READ_INTERNAL_ERROR 2


struct ndigo_packet {
    unsigned char channel;  // channel number ADC: 0-3 , 4=TDC, 5="Gate"-input
    unsigned char card;        // card number
    unsigned char type;        // signal type (adc, TDC or G)
    unsigned char flags;    // error handling .. overflows usw.
    unsigned int length;    // number of samples divided by 4.
    unsigned __int64 timestamp;        // pico sec. absolut time stamp of first bin
    unsigned __int64 data[1];        // raw data of ADC trace
};

typedef struct {

    /* The number of bytes occupied by the structure*/
    int size;
    /*	A version number that is increased when the definition of the structure is changed.
        The increment can be larger than one to match driver version numbers or similar. Set to 0 for all versions up to first release.
    */
    int version;

    /* Bandwidth. Currently fixed at 3G, might later be configurable to 1G (fullBW=false).*/
    double bandwidth;

    /*	Actual sample rate of currently sampled data. This is affected by the number of channels in use
        in the current ADC mode and possibly also by changes to the clock.
    */
    double sample_rate;
    /* The period one sample in the data represent in picoseconds */
    double sample_period;

    /* the ID the board should use to identify itself in the output data stream. 0 to 255. */
    int board_id;
    /* Number of channels in the current mode.*/
    int channels;

    /*

    */
    int channel_mask;

    /** The total amount of buffer in bytes*/
    __int64 total_buffer;

    __int64 free_buffer;
} ndigo_param_info;


struct ndigo_static_info {
    /*! \brief The number of bytes occupied by the structure
     */
    int size;

    /*! \brief A version number
     *
     * that is increased when the definition of the structure is changed.
     * The increment can be larger than one to match driver version numbers or similar.
     * Set to 0 for all versions up to first release.
     */
    int version;

    /*! \brief Index of the board as passed to the constructor
     *
     * or set via int @link conffuncts ndigo_set_board_id(ndigo_device *device, int board_id) @endlink.
     */
    int board_id;

    /*! \brief driver revision number
     *
     * The lower three bytes contain a triple level hierarchy of version numbers. * E.g. 0x010103 codes
     * version 1.1.3. A change in the first digit generally requires a recompilation of user applications.
     * Change in the second digit denote significant improvements or changes that don't break compatibility
     * and the third digit changes with minor bugfixes and the like.
     */
    int driver_revision;

    /*! \brief Revision number of the FPGA configuration.
     *
     * This can be read from a register.
     */
    int firmware_revision;

    /*! \brief board revision number
     *
     * This can be read from a register. It is a four bit number that changes when the schematic of the
     * board is changed.
     *  - 0: Experimental first board Version. Labeled Rev. 1
     *  - 2: First commercial Version. Labeled "Rev. 2"
     *  - 3: for the version produced starting in 2011 labeled "Rev. 3"
     */
    int board_revision;

    /*! \brief The same board schematic can be populated in multiple Variants.
     *
     * This is a four bit code that	can be read from a register.
     * - For board revision 0 this always reads 0.
     * - For board revision 2
     *		- bit 0 determines whether this board contains an 8-bit or 10-bit ADC
     *		- bit 1 determines whether differential inputs are used
     *		- bit 2 determines whether the tdc-oscillator is present
     *		- bit 3 = 1 signifies a special version of the board.
     * - For board revision 3
     *		- bit 2 determines input connectors (0: single ende, 1: differential)
     *		- for the other bits see user guide
    */
    int board_configuration;

    /*! \brief Number of bits of the ADC
     *
     * set to 0 if unknown. Should be 14.
     */
    int adc_resolution;

    /*! \brief Maximum sample rate.
     *	- 2:5e8 = 250Msps for the Ndigo250M,
     *	- 1:25e8 = 125Msps for the Ndigo125M
     *	- 5e9 = 5Gsps for the Ndigo5G
     */
    double nominal_sample_rate;

    /*! \brief analog bandwidth
     *
     *	- 1.25e8 for 125MHz for Ndigo250M
     *	- 3e9 for 3Ghz for Ndigo5G
     */
    double analog_bandwidth;

    /*! \brief chipID as read from the 16 bit adc chip id register at SPI address 0.
     *
     * his value should be cached.
     */
    int chip_id;

    /*! \brief Serial number with year and running number in 8.24 format.
     *
     * The number is identical to the one printed on the silvery sticker on the board.
     */
    int board_serial;

    /*! \brief 64bit serial number from the configuration flash chip
     */
    int flash_serial_low;

    /*! \brief 64bit serial number from the configuration flash chip
     */
    int flash_serial_high;

    /*! \brief If not 0 the driver found valid calibration data in the flash on the board and is using it.
     */
    int flash_valid;

    /*! \brief Returns false for the standard AC coupled Ndigo5G.
     *
     * Returns true for the Ndigo250M.
     */
    unsigned char dc_coupled;

    /*! \brief Subversion revision id of the FPGA configuration.
     *
     * This can be read from a register.
     */
    int subversion_revision;

    /*! \brief calibration date
     *
     * DIN EN ISO 8601 string YYYY-MM-DD HH:DD describing the time when the card was calibrated.
     */
    char calibration_date[20];
};


#define FADC8_HEADER_EVENT_ID_1_25G_ADC1        0x0
#define FADC8_HEADER_EVENT_ID_1_25G_ADC2        0x1
#define FADC8_HEADER_EVENT_ID_1_25G_ADC3        0x2
#define FADC8_HEADER_EVENT_ID_1_25G_ADC4        0x3
#define FADC8_HEADER_EVENT_ID_2_5G_ADC12        0x4
#define FADC8_HEADER_EVENT_ID_2_5G_ADC34        0x5
#define FADC8_HEADER_EVENT_ID_5G_ADC1234        0x7
#define FADC8_HEADER_EVENT_ID_TDC                0x8
#define FADC8_HEADER_EVENT_ID_DIRECT_MEMORY_START        0xc      // add 1.2.2011
#define FADC8_HEADER_EVENT_ID_DIRECT_MEMORY_STOP        0xd      // add 1.2.2011
#define FADC8_HEADER_EVENT_ID_ANALYZED_SIGNAL    0xe
#define FADC8_HEADER_EVENT_ID_END_MARKER        0xf


#ifndef FADC8_MANAGER_CLASS_IS_DEFINED
struct fADC8_signal_info_struct {
    __int8 ModuleIndex;
    __int8 ADCChipIndex;
    __int64 timestamp_in_ps; // unit = 1ps
    __int64 coarse_timestamp; // units: 4.8 ns
    __int8 signal_type;
    unsigned __int32 counter;
    double TDC_value_ps;
    __int32 signallength_including_header_in_32bit_words;
    __int8 timestamp_subindex;
    __int8 transition_type;
    __int16 adc_channel;
    __int8 TDC_subcounter;
    __int16 megasamples_per_second;
    __int8 is_a_synchronous_signal;
    //	__int32 i32dummy_for_future_use;
    //	unsigned __int32 header[4];
};
#endif


#define MAX_NUMBER_OF_BYTES_IN_POSTEVENTDATA 4000

class MyFILE {
public:
    MyFILE(bool mode_reading_) {
        error = 0;
        eof = false;
        mode_reading = mode_reading_;
        file = 0;
        position = 0;
        filesize = 0;
    }

    ~MyFILE() {
        close();
        error = 0;
        eof = false;
    }

    FILE *file;

    __int64 get_position() {
        if (!file) return 0;
        return position;
    }

    bool open(__int8 *name) {
        if (file) {
            error = 1;
            return false;
        }
        eof = false;
        if (mode_reading) {
            file = fopen(name, "rb");
            if (!file) {
                error = 1;
                return false;
            }
            if (ferror(file)) {
                fclose(file);
                error = 1;
                return false;
            }
            __int8 dummy;
            if (!fread(&dummy, 1, 1, file)) {
                fclose(file);
                error = 1;
                return false;
            }
            if (ferror(file)) {
                fclose(file);
                error = 1;
                return false;
            }
            _fseeki64(file, (unsigned __int64) 0, SEEK_END);
            filesize = _ftelli64(file);
            _fseeki64(file, (unsigned __int64) 0, SEEK_SET);
        } else file = fopen(name, "wb");
        if (file) return true;
        else {
            error = 1;
            return false;
        }
    }

    void close() {
        if (file) {
            fclose(file);
            file = 0;
        } else error = 1;
        position = 0;
        filesize = 0;
        eof = false;
    }

    unsigned __int64 tell() { return position; }

    void seek(unsigned __int64 pos);

    void read(__int8 *string, __int32 length_bytes) {
        unsigned __int32 read_bytes = (unsigned __int32) (fread(string, 1, length_bytes, file));
        if (__int32(read_bytes) != length_bytes) {
            error = 1;
            if (feof(file)) eof = true;
        }
        position += length_bytes;
    }

    void read(unsigned __int32 *dest, __int32 length_bytes) {
        unsigned __int32 read_bytes = (unsigned __int32) (fread(dest, 1, length_bytes, file));
        if (__int32(read_bytes) != length_bytes) {
            error = 1;
            if (feof(file)) eof = true;
        }
        position += length_bytes;
    }

    void write(const __int8 *string, __int32 length) {
        fwrite(string, 1, length, file);
        position += length;
        if (filesize < position) filesize = position;
    }

    void flush() { fflush(file); }

    MyFILE &operator>>(unsigned __int8 &c) {
        read((__int8 *) &c, sizeof(unsigned __int8));
        return *this;
    }

    MyFILE &operator>>(__int8 &c) {
        read((__int8 *) &c, sizeof(__int8));
        return *this;
    }

    MyFILE &operator>>(unsigned __int16 &l) {
        read((__int8 *) &l, sizeof(unsigned __int16));
        return *this;
    }

    MyFILE &operator>>(unsigned __int32 &l) {
        read((__int8 *) &l, sizeof(unsigned __int32));
        return *this;
    }

    MyFILE &operator>>(unsigned __int64 &l) {
        read((__int8 *) &l, sizeof(unsigned __int64));
        return *this;
    }

    MyFILE &operator>>(__int16 &s) {
        read((__int8 *) &s, sizeof(__int16));
        return *this;
    }

    MyFILE &operator>>(__int32 &l) {
        read((__int8 *) &l, sizeof(__int32));
        return *this;
    }

    MyFILE &operator>>(__int64 &l) {
        read((__int8 *) &l, sizeof(__int64));
        return *this;
    }

    MyFILE &operator>>(double &d) {
        read((__int8 *) &d, sizeof(double));
        return *this;
    }

    MyFILE &operator>>(bool &d) {
        read((__int8 *) &d, sizeof(bool));
        return *this;
    }

    MyFILE &operator<<(unsigned __int8 c) {
        write((__int8 *) &c, sizeof(unsigned __int8));
        return *this;
    }

    MyFILE &operator<<(__int8 c) {
        write((__int8 *) &c, sizeof(__int8));
        return *this;
    }

    MyFILE &operator<<(unsigned __int16 l) {
        write((__int8 *) &l, sizeof(unsigned __int16));
        return *this;
    }

    MyFILE &operator<<(unsigned __int32 l) {
        write((__int8 *) &l, sizeof(unsigned __int32));
        return *this;
    }

    MyFILE &operator<<(unsigned __int64 l) {
        write((__int8 *) &l, sizeof(unsigned __int64));
        return *this;
    }

    MyFILE &operator<<(__int16 s) {
        write((__int8 *) &s, sizeof(__int16));
        return *this;
    }

    MyFILE &operator<<(__int32 l) {
        write((__int8 *) &l, sizeof(__int32));
        return *this;
    }

    MyFILE &operator<<(__int64 l) {
        write((__int8 *) &l, sizeof(__int64));
        return *this;
    }

    MyFILE &operator<<(double d) {
        write((__int8 *) &d, sizeof(double));
        return *this;
    }

    MyFILE &operator<<(bool d) {
        write((__int8 *) &d, sizeof(bool));
        return *this;
    }

    __int32 error;
    bool eof;
    unsigned __int64 filesize;

private:
    bool mode_reading;
    unsigned __int64 position;
};


#ifndef _WINNT_
typedef union _myLARGE_INTEGER {
    struct {
        unsigned __int32 LowPart;
        __int32 HighPart;
    };
    struct {
        unsigned __int32 LowPart;
        __int32 HighPart;
    } u;
    __int64 QuadPart;
} myLARGE_INTEGER, *PmyLARGE_INTEGER;
#endif


#define DAQ_ID_HM1     0x000001
#define DAQ_ID_TDC8       0x000002
#define DAQ_ID_CAMAC   0x000003
#define DAQ_ID_2HM1       0x000004
#define DAQ_ID_2TDC8   0x000005
#define DAQ_ID_HM1_ABM 0x000006
#define DAQ_ID_TDC8HP  0x000008
#define DAQ_ID_TCPIP   0x000009
#define DAQ_ID_TDC8HPRAW 0x000010
#define DAQ_ID_FADC8    0x000011        // for fADC8
#define DAQ_ID_FADC4    0x000012        // for fADC4

#define DAQ_ID_RAW32BIT 100
#define DAQ_ID_SIMPLE 101


#define LM_BYTE                    1    //  8bit integer
#define LM_SHORT                2    // 16bit integer
#define LM_LONG                    3    // 32bit integer
#define    LM_FLOAT                4   // 32bit IEEE float
#define LM_DOUBLE                5    // 64bit IEEE float
#define LM_CAMAC                6    // 24bit integer
#define LM_DOUBLELONG            7    // 64bit integer
#define LM_SBYTE                8    // signed 8bit integer
#define LM_SSHORT                9    // signed 16bit integer
#define LM_SLONG                10    // signed 32bit integer
#define LM_SDOUBLELONG            11    // signed 64bit integer
#define LM_LASTKNOWNDATAFORMAT    LM_SDOUBLELONG
#define LM_USERDEF                -1    // user will handle the reading


struct TDC8PCI2_struct {
    __int32 i32NumberOfDAQLoops;
    __int32 GateDelay_1st_card;
    __int32 OpenTime_1st_card;
    __int32 WriteEmptyEvents_1st_card;
    __int32 TriggerFalling_1st_card;
    __int32 TriggerRising_1st_card;
    __int32 EmptyCounter_1st_card;
    __int32 EmptyCounter_since_last_Event_1st_card;
    bool use_normal_method;
    bool use_normal_method_2nd_card;
    __int32 sync_test_on_off;
    __int32 io_address_2nd_card;
    __int32 GateDelay_2nd_card;
    __int32 OpenTime_2nd_card;
    __int32 WriteEmptyEvents_2nd_card;
    __int32 TriggerFallingEdge_2nd_card;
    __int32 TriggerRisingEdge_2nd_card;
    __int32 EmptyCounter_2nd_card;
    __int32 EmptyCounter_since_last_Event_2nd_card;
    __int32 variable_event_length;
};


struct TDC8HP_info_struct {
    __int32 index;
    __int32 channelCount;
    __int32 channelStart;
    __int32 highResChannelCount;
    __int32 highResChannelStart;
    __int32 lowResChannelCount;
    __int32 lowResChannelStart;
    double resolution;
    __int32 serialNumber;
    __int32 version;
    __int32 fifoSize;
    __int32 *INLCorrection;
    unsigned __int16 *DNLData;
    bool flashValid;
};


struct TDC8HP_struct {
    __int32 exotic_file_type;
    __int32 no_config_file_read;
    unsigned __int64 RisingEnable_p61;
    unsigned __int64 FallingEnable_p62;
    __int32 TriggerEdge_p63;
    __int32 TriggerChannel_p64;
    __int32 OutputLevel_p65;
    __int32 GroupingEnable_p66;
    __int32 GroupingEnable_p66_output;
    __int32 AllowOverlap_p67;
    double TriggerDeadTime_p68;
    double GroupRangeStart_p69;
    double GroupRangeEnd_p70;
    __int32 ExternalClock_p71;
    __int32 OutputRollOvers_p72;
    __int32 DelayTap0_p73;
    __int32 DelayTap1_p74;
    __int32 DelayTap2_p75;
    __int32 DelayTap3_p76;
    __int32 INL_p80;
    __int32 DNL_p81;
    double OffsetTimeZeroChannel_s;
    __int32 BinsizeType;

    std::string csConfigFile, csINLFile, csDNLFile;

    __int32 csConfigFile_Length, csINLFile_Length, csDNLFile_Length;
    __int32 UserHeaderVersion;
    bool VHR_25ps;
    __int32 SyncValidationChannel;
    __int32 variable_event_length;
    bool SSEEnable, MMXEnable, DMAEnable;
    double GroupTimeOut;

    __int32 Number_of_TDCs;
    TDC8HP_info_struct *TDC_info[3];

//	bool	bdummy;
//	__int32	idummy;
//	double	ddummy;


    __int32 i32NumberOfDAQLoops;
    unsigned __int32 TDC8HP_DriverVersion;
    __int32 iTriggerChannelMask;
    __int32 iTime_zero_channel;


    __int32 number_of_bools;
    __int32 number_of_int32s;
    __int32 number_of_doubles;

    unsigned __int32 ui32oldRollOver;
    unsigned __int64 ui64RollOvers;
    unsigned __int32 ui32AbsoluteTimeStamp;
//	unsigned __int64 ui64OldTimeStamp;
    unsigned __int64 ui64TDC8HP_AbsoluteTimeStamp;

    __int32 channel_offset_for_rising_transitions;
};


struct fADC4_struct {
    unsigned __int32 driver_version;
    __int32 i32NumberOfADCmodules;
    __int32 i32NumberOfDAQLoops;
    bool bReadCustomData;
    __int32 iTriggerChannel; // first has index 0
    __int32 number_of_bools;
    __int32 number_of_int32s;
    __int32 number_of_uint32s;
    __int32 number_of_doubles;

    double dGroupRangeStart;
    double dGroupRangeEnd;

    __int32 GroupEndMarker;

    __int32 packet_count;
    ndigo_param_info ndigo_parameters[20];
    double threshold_GT[80];
    double threshold_LT[80];
    double GND_level[80];
    double set_DC_offset[80];
    __int32 sampling_mode[20];
    double bits_per_mVolt[20];
    std::string csConfigFile, csINLFile, csDNLFile;

    ndigo_static_info ndigo_info[10];
};


struct fADC8_struct {
    unsigned __int32 driver_version;

    __int32 i32NumberOfDAQLoops;

    __int32 number_of_bools;
    __int32 number_of_int32s;
    __int32 number_of_uint32s;
    __int32 number_of_doubles;

    __int32 GroupEndMarker;

    __int32 i32NumberOfADCmodules;
    __int32 iEnableGroupMode;
    __int32 iTriggerChannel; // first has index 0
    __int32 iPreSamplings_in_4800ps_units;
    __int32 iPostSamplings_in_9600ps_units;
    __int32 iEnableTDCinputs;
    __int32 iChannelMode[8][2]; // 0 = 1.25Gs, 1 = 2.5Gs, 2 = 5Gs
    __int32 iThreshold_GT[8][8];
    __int32 iThreshold_LT[8][8];
    __int32 iSynchronMode[8][2];
    bool bReadCustomData;

    __int32 firmware_version[8];
    __int32 serial_number[8];

    __int32 veto_gate_length;
    __int32 veto_delay_length;
    __int32 veto_mask;

    __int32 GND_level[8][10];

    double dGroupRangeStart;
    double dGroupRangeEnd;
    double dSyncTimeOffset[8][2];
    bool at_least_1_signal_was_written;
};


struct HM1_struct {
    __int32 FAK_DLL_Value;
    __int32 Resolution_Flag;
    __int32 trigger_mode_for_start;
    __int32 trigger_mode_for_stop;
    __int32 Even_open_time;
    __int32 Auto_Trigger;
    __int32 set_bits_for_GP1;
    __int32 ABM_m_xFrom;
    __int32 ABM_m_xTo;
    __int32 ABM_m_yFrom;
    __int32 ABM_m_yTo;
    __int32 ABM_m_xMin;
    __int32 ABM_m_xMax;
    __int32 ABM_m_yMin;
    __int32 ABM_m_yMax;
    __int32 ABM_m_xOffset;
    __int32 ABM_m_yOffset;
    __int32 ABM_m_zOffset;
    __int32 ABM_Mode;
    __int32 ABM_OsziDarkInvert;
    __int32 ABM_ErrorHisto;
    __int32 ABM_XShift;
    __int32 ABM_YShift;
    __int32 ABM_ZShift;
    __int32 ABM_ozShift;
    __int32 ABM_wdShift;
    __int32 ABM_ucLevelXY;
    __int32 ABM_ucLevelZ;
    __int32 ABM_uiABMXShift;
    __int32 ABM_uiABMYShift;
    __int32 ABM_uiABMZShift;
    bool use_normal_method;

    __int32 TWOHM1_FAK_DLL_Value;
    __int32 TWOHM1_Resolution_Flag;
    __int32 TWOHM1_trigger_mode_for_start;
    __int32 TWOHM1_trigger_mode_for_stop;
    __int32 TWOHM1_res_adjust;
    double TWOHM1_tdcresolution;
    __int32 TWOHM1_test_overflow;
    __int32 TWOHM1_number_of_channels;
    __int32 TWOHM1_number_of_hits;
    __int32 TWOHM1_set_bits_for_GP1;
    __int32 TWOHM1_HM1_ID_1;
    __int32 TWOHM1_HM1_ID_2;
};


class LMF_IO {
public:

    LMF_IO(__int32 Number_of_Channels, __int32 Number_of_Hits);

    ~LMF_IO();

    static __int32 GetVersionNumber();

    bool OpenInputLMF(__int8 *Filename);

    bool OpenInputLMF(std::string Filename);

    void CloseInputLMF();

    bool OpenOutputLMF(__int8 *Filename);

    bool OpenOutputLMF(std::string Filename);

    void CloseOutputLMF();

    unsigned __int64 GetLastLevelInfo();

    bool Clone(LMF_IO *clone);

    void WriteTDCData(double timestamp, unsigned __int32 cnt[], __int32 *tdc);

    void WriteTDCData(unsigned __int64 timestamp, unsigned __int32 cnt[], __int32 *tdc);

    void WriteTDCData(double timestamp, unsigned __int32 cnt[], double *tdc);

    void WriteTDCData(unsigned __int64 timestamp, unsigned __int32 cnt[], double *tdc);

    void WriteTDCData(double timestamp, unsigned __int32 cnt[], __int64 *tdc);

    void WriteTDCData(unsigned __int64 timestamp, unsigned __int32 cnt[], __int64 *tdc);

    void WriteTDCData(double timestamp, unsigned __int32 cnt[], unsigned __int16 *tdc);

    void WriteTDCData(unsigned __int64 timestamp, unsigned __int32 cnt[], unsigned __int16 *tdc);

    bool ReadNextfADC4packet(ndigo_packet *packet, bool &bEnd_of_group_detected, __int16 *i16buffer,
                             __int32 buffersize_in_16bit_words);

    bool ReadNextfADC8Signal(fADC8_signal_info_struct &signal_info, bool &bEnd_of_group_detected,
                             unsigned __int32 *ui32buffer, __int32 buffersize_in_32bit_words);

    bool WriteNextfADC8Signal(fADC8_signal_info_struct *signal_info, unsigned __int32 *ui32buffer);

    bool WritefADC8EndGroupMarker();

    void WriteFirstHeader();

    bool ReadNextEvent();

    bool ReadNextCAMACEvent();

    void GetNumberOfHitsArray(unsigned __int32 cnt[]);

    void GetNumberOfHitsArray(__int32 cnt[]);

    void GetTDCDataArray(__int32 *tdc);

    void GetTDCDataArray(__int64 *tdc);

    void GetTDCDataArray(double *tdc);

    void GetTDCDataArray(unsigned __int16 *tdc);

    void GetCAMACArray(unsigned __int32 []);

    void WriteCAMACArray(double, unsigned int[]);

    unsigned __int64 GetEventNumber();

    double GetDoubleTimeStamp();

    unsigned __int64 Getuint64TimeStamp();

    unsigned __int32 GetNumberOfChannels();

    unsigned __int32 GetMaxNumberOfHits();

    bool SeekToEventNumber(unsigned __int64 Eventnumber);

    const char *GetErrorText(__int32 error_id);

    void GetErrorText(__int32 error_id, __int8 char_buffer[]);

    void GetErrorText(__int8 char_buffer[]);

    __int32 GetErrorStatus();


    bool decompress_synchronous_fADC8_signal(fADC8_signal_info_struct &signal_info, unsigned __int32 source_buffer[],
                                             unsigned __int32 source_buffer_size_in_32bit_words,
                                             __int32 &number_of_filled_16bit_words,
                                             __int16 i16bit_target_buffer1[],
                                             __int32 target_buffer1_size_in_16bit_words,
                                             __int16 i16bit_target_buffer2[],
                                             __int32 target_buffer2_size_in_16bit_words,
                                             __int16 i16bit_target_buffer3[],
                                             __int32 target_buffer3_size_in_16bit_words,
                                             __int16 i16bit_target_buffer4[],
                                             __int32 target_buffer4_size_in_16bit_words);

    bool decompress_asynchronous_fADC8_signal(fADC8_signal_info_struct &signal_info, unsigned __int32 source_buffer[],
                                              unsigned __int32 source_buffer_size_in_32bit_words,
                                              __int16 i16bit_target_buffer[], __int32 target_buffer_size_in_16bit_words,
                                              __int32 &number_of_filled_16bit_words);

    void prepare_Cobold2008b_TDC8HP_header_output() { // Cobold2008 release August 2009
        //TDC8HP.TriggerDeadTime_p68;
        //TDC8HP.GroupRangeStart_p69;
        //TDC8HP.GroupRangeEnd_p70;
        //Starttime_output;
        //Stoptime_output;
        TDC8HP.FallingEnable_p62 = 1044991; // all 9 channels on 2 cards
        TDC8HP.TriggerChannel_p64 = 1;
        frequency = 1.e12; // 1ps
        tdcresolution_output = 0.025; // or 0.016 ?
        TDC8HP.VHR_25ps = true;
        TDC8HP.UserHeaderVersion = 7;
        DAQVersion_output = 20080507;
        Cobold_Header_version_output = 2008;
        system_timeout_output = 5; // obsolete
        time_reference_output = (__int32) int(Starttime_output);
        common_mode_output = 0; // 0 = common start
        data_format_in_userheader_output = -1;
        DAQ_ID_output = 8; // 8 = TDC8HP
        timestamp_format_output = 2;
        LMF_Version_output = 10;
        IOaddress = 0;
        number_of_DAQ_source_strings_output = 0;
        TDC8HP.OutputRollOvers_p72 = 1;
        TDC8HP.Number_of_TDCs = 0;
        TDC8HP.number_of_int32s = 5;
        TDC8HP.number_of_bools = 0;
        TDC8HP.number_of_doubles = 1;
        TDC8HP.GroupingEnable_p66_output = false;
    }

private:
    void Initialize();

    void write_times(MyFILE *, time_t, time_t);

    bool OpenNonCoboldFile(void);

    __int32 ReadCAMACHeader();

    __int32 ReadTDC8PCI2Header();

    __int32 Read2TDC8PCI2Header();

    __int32 ReadTDC8HPHeader_LMFV_1_to_7(__int32 byte_counter_external);

    __int32 ReadTDC8HPHeader_LMFV_8_to_9();

    __int32 ReadTDC8HPHeader_LMFV_10();

    __int32 ReadHM1Header();

    __int32 ReadTCPIPHeader();

    __int32 ReadfADC8Header();

    __int32 WritefADC8_header_LMFversion10();

    __int32 ReadfADC8_header_LMFversion10();

    __int32 ReadfADC8_header_LMFversion9();

    __int32 ReadfADC4Header_up_to_v11();

    void WriteEventHeader(unsigned __int64 timestamp, unsigned __int32 cnt[]);

    __int32 WriteCAMACHeader();

    __int32 WriteTDC8PCI2Header();

    __int32 Write2TDC8PCI2Header();

    __int32 WriteTDC8HPHeader_LMFV_1_to_7();

    __int32 WriteTDC8HPHeader_LMFV_8_to_9();

    __int32 WriteTDC8HPHeader_LMFV_10_to_12();

    __int32 WriteHM1Header();

    __int32 WriteTCPIPHeader();

    bool Read_TDC8HP_raw_format(unsigned __int64 &ui64TDC8HP_AbsoluteTimeStamp);

    __int32 PCIGetTDC_TDC8HP_25psGroupMode(unsigned __int64 &ui64TDC8HPAbsoluteTimeStamp, __int32 count,
                                           unsigned __int32 *Buffer);


public:
    std::string Versionstring;
    std::string FilePathName;
    std::string OutputFilePathName;
    std::string Comment;
    std::string Comment_output;
    std::string DAQ_info;
    std::string Camac_CIF;

    char *error_text[40];

    double *Parameter;
    double *Parameter_old;

    time_t Starttime;
    time_t Stoptime;
    time_t Starttime_output;
    time_t Stoptime_output;

    __int32 time_reference;
    __int32 time_reference_output;

    unsigned __int32 ArchiveFlag;
    __int32 Cobold_Header_version;
    __int32 Cobold_Header_version_output;

    unsigned __int64 uint64_LMF_EventCounter;
    unsigned __int64 uint64_number_of_read_events;
    unsigned __int64 uint64_Numberofevents;

    __int32 Numberofcoordinates;
    __int32 CTime_version, CTime_version_output;
    unsigned __int32 SIMPLE_DAQ_ID_Orignial;
    __int32 DAQVersion;
    __int32 DAQVersion_output;
    unsigned __int32 DAQ_ID;
    unsigned __int32 DAQ_ID_output;
    __int32 data_format_in_userheader;
    __int32 data_format_in_userheader_output;

    unsigned __int32 Headersize;
    unsigned __int32 User_header_size;
    unsigned __int32 User_header_size_output;

    __int32 IOaddress;
    __int32 timestamp_format;
    __int32 timestamp_format_output;
    __int32 timerange;

    unsigned __int32 number_of_channels;
    unsigned __int32 number_of_channels2;
    unsigned __int32 max_number_of_hits;
    unsigned __int32 max_number_of_hits2;

    __int32 number_of_channels_output;
    __int32 number_of_channels2_output;
    __int32 max_number_of_hits_output;
    __int32 max_number_of_hits2_output;

    unsigned __int64 ui64LevelInfo;

    unsigned __int32 changed_mask_read;

    unsigned __int8 ui8_PostEventData[MAX_NUMBER_OF_BYTES_IN_POSTEVENTDATA];
    __int32 number_of_bytes_in_PostEventData;

    __int32 DAQSubVersion;
    __int32 module_2nd;
    __int32 system_timeout;
    __int32 system_timeout_output;
    __int32 common_mode;
    __int32 common_mode_output;
    __int32 DAQ_info_Length;
    __int32 Camac_CIF_Length;
    __int32 LMF_Version;
    __int32 LMF_Version_output;
    __int32 TDCDataType;

    __int32 iLMFcompression;
    unsigned __int32 LMF_Header_version;

    double tdcresolution;
    double tdcresolution_output;
    double frequency;
    double DOUBLE_timestamp;
    unsigned __int64 ui64_timestamp;

    __int32 errorflag;
    bool skip_header;

    unsigned __int32 DAQ_SOURCE_CODE_bitmasked;
    unsigned __int32 DAN_SOURCE_CODE_bitmasked;
    unsigned __int32 CCF_HISTORY_CODE_bitmasked;

    __int32 number_of_CCFHistory_strings;
    __int32 number_of_CCFHistory_strings_output;
    std::string **CCFHistory_strings;
    std::string **CCFHistory_strings_output;
    __int32 number_of_DAN_source_strings;
    __int32 number_of_DAN_source_strings_output;
    std::string **DAN_source_strings;
    std::string **DAN_source_strings_output;
    __int32 number_of_DAQ_source_strings;
    __int32 number_of_DAQ_source_strings_output;
    std::string **DAQ_source_strings;
    std::string **DAQ_source_strings_output;

    HM1_struct HM1;
    TDC8HP_struct TDC8HP;
    TDC8PCI2_struct TDC8PCI2;
    fADC8_struct fADC8;
    fADC4_struct fADC4;

    unsigned __int64 uint64_number_of_written_events;

    MyFILE *input_lmf;
    MyFILE *output_lmf;

    bool InputFileIsOpen;
    bool OutputFileIsOpen;

private:
    unsigned __int32 *ui32buffer;
    __int32 ui32buffer_size;
    bool not_Cobold_LMF;
    unsigned __int32 Headersize_output;
    __int32 output_byte_counter;
    __int32 Numberofcoordinates_output;
    bool must_read_first;
    unsigned __int32 *number_of_hits;
    __int32 *i32TDC;
    unsigned __int16 *us16TDC;
    double *dTDC;

    __int32 num_channels;
    __int32 num_ions;


    unsigned __int32 *CAMAC_Data;
};

#endif