// ----------------------------------------------------------------------------
// TMC429.h
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------

#ifndef TMC429_H
#define TMC429_H
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "SPI.h"

#include "Streaming.h"


class TMC429
{
public:
  TMC429(const size_t cs_pin);

  void setup();

  uint32_t getVersion();
  bool checkVersion();

  void specifyClockFrequencyInMHz(uint8_t clock_frequency);
  double getStepTimeInMicroSeconds();

  uint32_t getVelocityMaxMaxInHz();
  void setVelocityScaleUsingTotalMaxInHz(const uint32_t velocity);

  uint32_t getPositionTarget(const size_t motor);
  void setPositionTarget(const size_t motor, const uint32_t position);

  uint32_t getPositionActual(const size_t motor);
  void setPositionActual(const size_t motor, const uint32_t position);

  uint32_t getVelocityMinInHz(const size_t motor);
  void setVelocityMinInHz(const size_t motor, const uint32_t velocity);

  uint32_t getVelocityMaxInHz(const size_t motor);
  void setVelocityMaxInHz(const size_t motor, const uint32_t velocity);

  int32_t getVelocityTargetInHz(const size_t motor);
  void setVelocityTargetInHz(const size_t motor, const int32_t velocity);

  int32_t getVelocityActualInHz(const size_t motor);

  struct Status
  {
    uint8_t at_target_position_0 : 1;
    uint8_t switch_left_0 : 1;
    uint8_t at_target_position_1 : 1;
    uint8_t switch_left_1 : 1;
    uint8_t at_target_position_2 : 1;
    uint8_t switch_left_2 : 1;
    uint8_t cdgw : 1;
    uint8_t interrupt : 1;
  };

  Status getStatus();

  enum Mode
    {
      RAMP_MODE=0b00,
      SOFT_MODE=0b01,
      VELOCITY_MODE=0b10,
      HOLD_MODE=0b11,
    };

  Mode getMode(const size_t motor);
  void setMode(const size_t motor, const Mode mode);

  struct ReferenceConfiguration
  {
    uint8_t disable_stop_l : 1;
    uint8_t disable_stop_r : 1;
    uint8_t soft_stop : 1;
    uint8_t ref_rnl : 1;
    uint8_t space : 4;
  };

  ReferenceConfiguration getReferenceConfiguration(const size_t motor);

  void enableLeftSwitchStop(const size_t motor);
  void disableLeftSwitchStop(const size_t motor);

  void enableRightSwitchStop(const size_t motor);
  void disableRightSwitchStop(const size_t motor);

  void enableSoftStop(const size_t motor);
  void disableSoftStop(const size_t motor);

  void setReferenceSwitchToLeft(const size_t motor);
  void setReferenceSwitchToRight(const size_t motor);

  bool positionLatched(const size_t motor);

  struct InterfaceConfiguration
  {
    uint8_t inv_ref : 1;
    uint8_t sdo_int : 1;
    uint8_t step_half : 1;
    uint8_t inv_stp : 1;
    uint8_t inv_dir : 1;
    uint8_t en_sd : 1;
    uint8_t pos_comp_sel : 2;
    uint8_t en_refr : 1;
    uint8_t space : 7;
  };

  InterfaceConfiguration getInterfaceConfiguration();

  void setReferenceActiveLow();
  void setReferenceActiveHigh();

  void enableInverseStepPolarity();
  void disableInverseStepPolarity();

  void enableInverseDirPolarity();
  void disableInverseDirPolarity();

  void setStepDirOutput();
  void setSpiOutput();

  void setPositionCompareMotor(const size_t motor);

  void enableRightReferences();
  void disableRightReferences();

  struct SwitchState
  {
    uint8_t r0 : 1;
    uint8_t l0 : 1;
    uint8_t r1 : 1;
    uint8_t l1 : 1;
    uint8_t r2 : 1;
    uint8_t l2 : 1;
    uint8_t space : 2;
  };

  SwitchState getSwitchState();

  struct ClockConfiguration
  {
    uint8_t usrs : 3;
    uint8_t space0 : 5;
    uint8_t ramp_div : 4;
    uint8_t pulse_div : 4;
  };

  ClockConfiguration getClockConfiguration();

private:
  // SPISettings
  const static uint32_t SPI_CLOCK = 1000000;
  const static uint8_t SPI_BIT_ORDER = MSBFIRST;
  const static uint8_t SPI_MODE = SPI_MODE3;

  const static uint8_t MOTOR_COUNT = 3;

  const static uint32_t VERSION = 0x429101;

  Status status_;
  uint8_t clock_frequency_;
  const static uint8_t CLOCK_FREQUENCY_MAX = 32;
  uint8_t pulse_div_;
  const static uint8_t PULSE_DIV_MAX = 13;
  const static uint32_t MHZ_PER_HZ = 1000000;
  const static uint32_t VELOCITY_CONSTANT = 65536;
  const static uint32_t VELOCITY_REGISTER_MAX = 2047;

  // MOSI Datagrams
  union MosiDatagram
  {
    struct Fields
    {
      uint32_t data : 24;
      uint8_t rw : 1;
      uint8_t address : 4;
      uint8_t smda : 2;
      uint8_t rrs : 1;
    } fields;
    uint32_t uint32;
  };
  const static uint8_t RW_READ = 1;
  const static uint8_t RW_WRITE = 0;

  // IDX Addresses
  const static uint8_t ADDRESS_X_TARGET = 0b0000;
  const static uint8_t ADDRESS_X_ACTUAL = 0b0001;
  const static uint8_t ADDRESS_V_MIN = 0b0010;
  const static uint8_t ADDRESS_V_MAX = 0b0011;
  const static uint8_t ADDRESS_V_TARGET = 0b0100;
  const static uint8_t ADDRESS_V_ACTUAL = 0b0101;
  const static uint8_t ADDRESS_A_MAX = 0b0110;
  const static uint8_t ADDRESS_A_ACTUAL = 0b0111;
  const static uint8_t ADDRESS_A_THRESHOLD = 0b1000;
  const static uint8_t ADDRESS_P_FACTOR = 0b1001;
  const static uint8_t ADDRESS_REF_CONF_MODE = 0b1010;
  const static uint8_t ADDRESS_INTERRUPT = 0b1011;
  const static uint8_t ADDRESS_CLOCK_CONFIGURATION = 0b1100;
  const static uint8_t ADDRESS_DX_REF_TOLERANCE = 0b1101;
  const static uint8_t ADDRESS_X_LATCHED = 0b1110;
  const static uint8_t ADDRESS_USTEP_COUNT_429 = 0b1111;

  // JDX Addresses
  const static uint8_t ADDRESS_DATAGRAM_LOW_WORD = 0b0000;
  const static uint8_t ADDRESS_DATAGRAM_HIGH_WORD = 0b0001;
  const static uint8_t ADDRESS_COVER_POS_LEN = 0b0010;
  const static uint8_t ADDRESS_COVER_DATAGRAM = 0b0011;
  const static uint8_t ADDRESS_IF_CONFIGURATION_429 = 0b0100;
  const static uint8_t ADDRESS_POS_COMP_429 = 0b0101;
  const static uint8_t ADDRESS_POS_COMP_INT = 0b0110;
  const static uint8_t ADDRESS_POWER_DOWN = 0b1000;
  const static uint8_t ADDRESS_TYPE_VERSION_429 = 0b1001;
  const static uint8_t ADDRESS_SWITCHES = 0b1110;
  const static uint8_t ADDRESS_GLOBAL_PARAMETERS = 0b1111;

  const static uint8_t SMDA_COMMON = 0b11;

  const static uint8_t RRS_REGISTER = 0;
  const static uint8_t RRS_RAM = 1;

  // MISO Datagrams
  union MisoDatagram
  {
    struct Fields
    {
      uint32_t data : 24;
      Status status;
    } fields;
    uint32_t uint32;
  };

  // Masks
  const static uint32_t X_MASK = 0xffffff;
  const static uint32_t V_MIN_MAX_MASK = 0x7ff;
  const static uint32_t V_MASK = 0xfff;
  const static uint32_t A_MAX_MASK = 0x7ff;
  const static uint32_t A_MASK = 0xfff;

  // union structs
  union PFactor
  {
    struct Fields
    {
      uint8_t pdiv : 4;
      uint8_t space0 : 4;
      uint8_t pmul : 7;
      uint8_t one : 1;
      uint8_t space1 : 8;
      uint8_t space2 : 8;
    } fields;
    uint32_t uint32;
  };
  union RefConfMode
  {
    struct Fields
    {
      uint8_t mode : 2;
      uint8_t space0 : 6;
      ReferenceConfiguration ref_conf;
      uint8_t lp : 1;
      uint8_t space1 : 7;
      uint8_t space2 : 8;
    } fields;
    uint32_t uint32;
  };
  union IfConf
  {
    struct Fields
    {
      InterfaceConfiguration if_conf;
      uint16_t space0 : 16;
    } fields;
    uint32_t uint32;
  };
  union SwState
  {
    struct Fields
    {
      SwitchState switch_state;
      uint16_t space0 : 16;
    } fields;
    uint32_t uint32;
  };
  union GlobalParameters
  {
    struct Fields
    {
      uint8_t lsmd : 2;
      uint8_t nscs_s : 1;
      uint8_t sck_s : 1;
      uint8_t ph_ab : 1;
      uint8_t fd_ab : 1;
      uint8_t dac_ab : 1;
      uint8_t cs_com_ind : 1;
      uint8_t clk2_div : 8;
      uint8_t cont_update : 1;
      uint8_t space0 : 3;
      uint8_t ref_mux : 1;
      uint8_t mot1r : 1;
      uint8_t space1 : 2;
    } fields;
    uint32_t uint32;
  };
  union ClkConfig
  {
    struct Fields
    {
      ClockConfiguration clk_config;
      uint8_t space0 : 8;
    } fields;
    uint32_t uint32;
  };

  size_t cs_pin_;

  uint32_t readRegister(const uint8_t smda, const uint8_t address);
  void writeRegister(const uint8_t smda, const uint8_t address, const uint32_t data);
  MisoDatagram writeRead(const MosiDatagram datagram_write);

  int32_t convertVelocityToHz(const int16_t velocity);
  int16_t convertVelocityFromHz(const int32_t velocity);
  void setOptimalPulseDiv(const uint32_t velocity_max);

  uint16_t getVelocityMin(const size_t motor);
  void setVelocityMin(const size_t motor, const uint16_t velocity);

  uint16_t getVelocityMax(const size_t motor);
  void setVelocityMax(const size_t motor, const uint16_t velocity);

  int16_t getVelocityTarget(const size_t motor);
  void setVelocityTarget(const size_t motor, const int16_t velocity);

  int16_t getVelocityActual(const size_t motor);

};

#endif
