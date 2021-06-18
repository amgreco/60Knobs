/*
   This file includes all the code associated to the prototypes defined in "functions.h"
*/

//Averages the knob position and decides if a message should be outputed, if yes, decodes the current preset and sends the acording message
void interpretKnob(uint8_t index, bool force, bool inhibit) {

  // read averaged position of knob
  uint16_t toSend = getKnobValue(index);

  //if the value to send is relevant, we send it to the MIDI OUT port
  if (((toSend != emittedValue[0][index]) && (toSend != emittedValue[1][index]) && (toSend != emittedValue[2][index])) || (force == true)) {  //if a message should be sent

  // Check knob type :
  // 0 - Disabled 
  // 1 - Global CC
  // 2 - Independent CC
  // 3 - NRPN
  // 4 - DX7
  // 5 - Reface DX
  // 6 - DSI Evolver
  // 7 - DSI Mopho


  switch (activePreset.knobType){
    case 1 : // CC 
    //Test if global or independent channel
      if (activePreset.knobInfo[index].SYSEX == 0){
        sendGlobalCC(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, toSend, activePreset.channel);
      }
      else {
        sendIndeCC(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, toSend, activePreset.knobInfo[index].SYSEX);
      }
    break;
    case 2 : // NRPN
      sendNRPN(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, activePreset.knobInfo[index].SYSEX, toSend, activePreset.channel);
    break;
    case 3 : // DX7
      sendDX7Message(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, activePreset.knobInfo[index].SYSEX, toSend); 
    break;
    case 4 : // REFACE DX
      sendRefaceDXMessage(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, activePreset.knobInfo[index].SYSEX, toSend); 
    break;
    case 5 : // EVOLVER 
      sendEvolverMessage(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, activePreset.knobInfo[index].SYSEX, toSend); 
    break;
    case 6 : // MOPHO
      sendMophoNRPN(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, activePreset.knobInfo[index].SYSEX, toSend, activePreset.channel);
    break; 
    case 7 : //SID
      sendSIDNRPN(activePreset.knobInfo[index].CC, activePreset.knobInfo[index].NRPN, activePreset.knobInfo[index].SYSEX, toSend, activePreset.channel);
    break;
  }

    //we fill the emission buffers
    for (int i = 2; i > 0; i--) {
      emittedValue[i][index] = emittedValue[i - 1][index];
    }
    emittedValue[0][index] = toSend;
  }
}

void sendGlobalCC(uint8_t param, uint8_t range, uint16_t value, uint8_t channel){ 
  value = map(value, 0, KNOB_RES, 0, range); 
  MIDI.sendControlChange(param,value,channel);

}

void sendIndeCC(uint8_t param, uint8_t range, uint16_t value, uint8_t channel){
  value = map(value, 0, KNOB_RES, 0, range); 
  MIDI.sendControlChange(param,value,channel);

}
  
void sendNRPN(uint8_t paramMSB, uint8_t paramLSB, uint8_t range, uint16_t value, uint8_t channel) {
  //map values range
  value = map(value,0,KNOB_RES,0,range);

  //Send message
  MIDI.sendControlChange(99, paramMSB, channel);
  MIDI.sendControlChange(98, paramLSB, channel); 
  MIDI.sendControlChange(6, value / 128 , channel);  
  MIDI.sendControlChange(38, value & 0x7F , channel);
}


void sendDX7Message(uint8_t paramMSB, uint8_t paramLSB, uint8_t range, uint16_t value) {
  //the array to transmit
  uint8_t data[5] = {0x43, 0x10, 0x00, 0x00, 0x00};

  value = map(value, 0, KNOB_RES, 0, range);
  data[2] = paramMSB;
  data[3] = paramLSB;
  data[4] = value;

  //output the Sysex message
  MIDI.sendSysEx(5, data, false);

  //sync all the voices
  MIDI.sendControlChange(127, 0, 1);  //cc 127 on channel 1 (DX7 only sensitive on channel 1)
}

void sendRefaceDXMessage(uint8_t paramMSB, uint8_t paramLSB, uint8_t range, uint16_t value) {
  //the array to transmit
  uint8_t data[9] = {0x43, 0x10, 0x7F, 0x1C, 0x05, 0x30, 0x00, 0x00, 0x00};

  //if the parameter number takes more than seven bits, set the MSBit correctly
  uint8_t paramNBR = paramMSB + paramLSB;
  if (paramNBR > 35 && paramNBR < 140) {
    paramNBR -= 36;
    uint8_t _operator = paramNBR / 26;
    data[5] = 0x31;
    data[6] = _operator;
    data[7] = paramNBR % 26;
  }
  else if (paramNBR <= 34) {
    data[5] = 0x30;
    data[6] = 0x00;
    data[7] = paramNBR;
  }

  value = map(value, 0, KNOB_RES, 0, range);
  data[8] = value;

  MIDI.sendSysEx(9, data, false);
}


void sendEvolverMessage(uint8_t control, uint8_t rangeLS, uint8_t rangeMS, uint16_t value) {
  //the array to transmit
  uint8_t data[7] = {0x01, 0x20, 0x01, 0x01, 0x00, 0x00, 0x00};

  value = map(value,0,KNOB_RES,0,rangeLS+rangeMS);

  data[4] = control;
  data[5] = value & 0x0F; // LS Nibble
  data[6] = ((value & 0xF0) >> 4); // MS Nibble

  MIDI.sendSysEx(7, data, false); 
}


void sendMophoNRPN(uint8_t paramMSB, uint8_t paramLSB, uint8_t range, uint16_t value, uint8_t channel) {
  //map values range
  value = map(value,0,KNOB_RES,0,range);

  //Send message
  MIDI.sendControlChange(99, paramMSB, channel);
  MIDI.sendControlChange(98, paramLSB, channel); 
  MIDI.sendControlChange(6, value / 128 , channel);  
  MIDI.sendControlChange(38, value & 0x7F , channel);
}

void sendSIDNRPN(uint8_t paramMSB, uint8_t paramLSB, uint16_t range, uint16_t value, uint8_t channel) {
  
  //map values range
  value = map(value, 0, KNOB_RES, 0, range);

  //Send message
  MIDI.sendControlChange(99, paramMSB +64, channel); //+64 is to access absolute values on MIDIBOX SID
  MIDI.sendControlChange(98, paramLSB, channel); 
  MIDI.sendControlChange(6, value / 128 , channel);  
  MIDI.sendControlChange(38, value % 128 , channel);
}


//Handles the "menu" system, what to do when the button is pressed
void renderFunctionButton() {
  if (!digitalRead(BUTTON_PIN)) {

    //We dont want any messages to be transmitted while the button is down
    MIDI.turnThruOff();

    //check if the button has been double pressed, if yes : force the emission of the messages associated to each knob
    if ((abs(millis() - lastButtonPress) > 100) && (abs(millis() - lastButtonPress) < 350)) {
      //show the user that the double click worked
      digitalWrite(LED_PIN, LOW);
      //sync the knobs
      for (uint8_t i = 0; i < NUMBEROFKNOBS; i++) {
        interpretKnob(i, true, false);
      }
      delay(500); //just for the hell of it, more visual perhaps
    }

    //we send an empty sysex message to ensure the connection between the 60K and the target
    MIDI.sendSysEx(0, NULL, false);

    //update the timing variable for the double press detection
    lastButtonPress = millis();

    while (!digitalRead(BUTTON_PIN)) { //while the button is held down

      //we detect the movement on the knobs we care for (0 to 15 and 50 to 54)
      for (int currentKnob = 0; currentKnob < NUMBEROFKNOBS; currentKnob++) {
        MIDI.read();
        selectKnob(currentKnob);  //Sets up the MUXs to direct the right knob to the analog input
        MIDI.read();
        updateKnob(currentKnob);  //fills the buffer, to know which knobs have moved
      }


      //do we need to change the channel ?
      for (uint8_t channelKnob = 0; channelKnob < 16; channelKnob++) {
        //if one the knobs associated to the MIDI channel selection has moved enough
        if (abs(knobBuffer[0][channelKnob] - knobBuffer[1][channelKnob]) == KnobSelectThreshold) {
          activePreset.channel = channelKnob + 1;
          //give a visual feedback to prove that the channel has changed
          digitalWrite(LED_PIN, LOW);
          delay(100);
          digitalWrite(LED_PIN, HIGH);
        }
      }

      //do we need to change preset ?
      for (uint8_t presetKnob = 50; presetKnob < 55; presetKnob++) {
        //if one of the knobs associated with the preset selection has moved enough
        if (abs(knobBuffer[0][presetKnob] - knobBuffer[1][presetKnob]) == KnobSelectThreshold) {
          loadPreset(presetKnob - 50);
          //give a visual feedback to prove that the preset has changed
          digitalWrite(LED_PIN, LOW);
          delay(250);
          digitalWrite(LED_PIN, HIGH);
        }
      }

      //we exit the "menu"so we can turn the MIDI thru back on
      MIDI.turnThruOn();
    }
  }
}




//return 0 if not inverted, not 0 otherwise
uint64_t isInverted(uint8_t index) {
  uint64_t offset = (uint64_t)index;
  return (activePreset.invertBits & ((uint64_t)1 << offset)) >> offset;
}

void clearBits64(uint64_t & value, uint8_t index) {
  value = value & ~(((uint64_t)1) << index);
}
