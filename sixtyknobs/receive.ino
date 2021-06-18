//Is executed everytime a Sysex message is received
void sysExInterpreter(byte* data, unsigned messageLength) {

  //check if this is a valid Sysex message
  if (data[MANUFACTURER] == BASTL_MANUFACTURER_ID) {
    //check the command byte and acts accordingly
    switch (data[COMMAND]) {



      case SETKNOBASCC :   //Sets a knob as an independent CC knob
        {
          activePreset.knobType = 1;
          //PARAM 1 : which knob do we affect ?
          //PARAM 2 : CC number
          //PARAM 3 : range
          //PARAM 4 : the MIDI channel of that knob
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = data[PARAM2];
            activePreset.knobInfo[knobIndex].NRPN = data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = data[PARAM4];;

            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }     


      case SETKNOBASNRPN : //Sets a knob as a Bipolar NRPN (-63, +63) knob
        {
          activePreset.knobType = 2;
          //PARAM 1 : which knob do we affect ?
          //PARAM 2 : NRPN number LSB
          //PARAM 3 : NRPN number MSB
          //PARAM 4 : NRPN range LSB
          //PARAM 5 : NRPN range MSB
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = 0;
            activePreset.knobInfo[knobIndex].NRPN = data[PARAM2] + data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = data[PARAM4] + data[PARAM5];

            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }
    

      case SETKNOBASDX :  //Sets a knob as a DX7 parameter change knob
        {
          activePreset.knobType = 3;
          //PARAM 1 : which knob do we affect ?
          //PARAM 2 : DX7 parameter number most significant bit
          //PARAM 3 : DX7 parameter number last 7 bits
          //PARAM 4 : maximum value that can be reached by that parameter
          
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = data[PARAM2];
            activePreset.knobInfo[knobIndex].NRPN =  data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = data[PARAM4];
            
            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }

      case SETKNOBASREFACEDX :  //Sets a knob as a DX7 parameter change knob
        {
          activePreset.knobType = 4;
          //PARAM 1 : which knob do we affect ?
          //PARAM 2 : DX7 parameter number most significant bit
          //PARAM 3 : DX7 parameter number last 7 bits
          //PARAM 4 : maximum value that can be reached by that parameter
          
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = data[PARAM2];
            activePreset.knobInfo[knobIndex].NRPN =  data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = data[PARAM4];
            
            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }


      case SETKNOBASEVO :  //Sets a knob as a Evolver parameter change knob
        {
          activePreset.knobType = 5;
          //PARAM 1 : Knob to affect 
          //PARAM 2 : Control Number
          //PARAM 3 : Range MS Nibble 
          //PARAM 4 : Range LS Nibble 
          
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = data[PARAM2];
            activePreset.knobInfo[knobIndex].NRPN = data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = data[PARAM4]; 
                                    
            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }        
        
      case SETKNOBASMOPHO :  //Sets a knob as a Mopho parameter change knob
        {
          activePreset.knobType = 6;
          //PARAM 1 : which knob do we affect ?
          //PARAM 2 : NRPN parameter number MSB
          //PARAM 3 : NRPN parameter number LSB
          //PARAM 4 : NRPN range MSB
          //PARAM 5 : NRPN range LSB
          
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = data[PARAM2];
            activePreset.knobInfo[knobIndex].NRPN = data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = (data[PARAM4] << 7 ) + data[PARAM5]; //calculate range 

            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }   
           
case SETKNOBASSID :  //Sets a knob as a SID  parameter change knob
        {
          activePreset.knobType = 7;
          //PARAM 1 : which knob do we affect ?
          //PARAM 2 : NRPN parameter number MSB
          //PARAM 3 : NRPN parameter number LSB
          //PARAM 4 : NRPN range MSB
          //PARAM 5 : NRPN range LSB
          
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC = data[PARAM2];
            activePreset.knobInfo[knobIndex].NRPN = data[PARAM3];
            activePreset.knobInfo[knobIndex].SYSEX = (data[PARAM4] << 7 ) + data[PARAM5]; //calculate range  

            //knob in normal mode by default
            clearBits64(activePreset.invertBits, data[PARAM1]);
          }

          break;
        }      
        
      case DISABLEKNOB :  //Sets a knob as an inactive CC knob
        {
          //PARAM 1 : which knob do we affect ?
          if (data[PARAM1] < NUMBEROFKNOBS) {
            uint8_t knobIndex = data[PARAM1];
            activePreset.knobInfo[knobIndex].CC  = 0; 
            activePreset.knobInfo[knobIndex].NRPN = 0;
            activePreset.knobInfo[knobIndex].SYSEX = 0; 
          }

          break;
        }

        
      case INVERTKNOB : //Sets a knob to be inverted or not
        {
          //PARAM1 : which knob do we affect ?
          //PARAM2 : 0-> knob in normal mode; 1-> knob in invert mode
          if (data[PARAM1] < NUMBEROFKNOBS) {
            if (data[PARAM2] == 0) {
              clearBits64(activePreset.invertBits, data[PARAM1]);
            }
            else {
              activePreset.invertBits |=  (((uint64_t)1) << data[PARAM1]);
              //bitSet(invertBits, data[PARAM1]);
            }
          }
          break;
        }


      case DROPNRPNLSB :
        {
          if(data[PARAM1] > 0) {
            activePreset.dropNRPNLSBvalue = true;        
          } else {
            activePreset.dropNRPNLSBvalue = false;
          }
        }

      case PRESETSAVE : //Saves the current state of the machine to the specified slot
        {
          //PARAM 1 : the slot where the preset will be saved

          if (data[PARAM1] < 5) {
            savePreset(data[PARAM1]);
          }
          break;
        }

      case PRESETLOAD : //Sets the state of the machine according to the specified preset
        {
          //PARAM 1 : number of the preset to load

          if (data[PARAM1] < 5) {
            loadPreset(data[PARAM1]);
          }

          break;
        }

      case SYNCKNOBS :  //Forces the emission of the messages associated to every knob
        {
          //NO PARAM
          for (uint8_t i = 0; i < NUMBEROFKNOBS; i++) {
            interpretKnob(i, true, false);
          }

          break;
        }

      case CHANNELCHANGE :  //Changes the global MIDI channel according to the one specified
        {
          //PARAM 1 : MIDI channel  (1 to 16)
          if (data[PARAM1] < 17 && data[PARAM1] > 0) {
            activePreset.channel = data[PARAM1];
          }

          break;
        }

      case RANDOMIZER : //Sends random CC messages forthe CC range specified
        {
          //PARAM 1 : Min CC number
          //PARAM 2 : Max CC number
          if (data[PARAM1] < 127 && data[PARAM2] >= data[PARAM1]) {
            for (int i = data[PARAM1]; i <= data[PARAM2]; i++) {
              MIDI.sendControlChange(i, random(0, 127), activePreset.channel);
            }

          }
          break;
        }
    }
  }
}

//Is executed everytime a MIDI Program Change message is received
//loads the specified preset
void handleProgramChange(byte channel, byte number) {
  if (number <= NUMBEROFPRESETS) {
    loadPreset(number);
  }
}
