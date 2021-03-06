#define UUID_LENGTH 16 // UUIDs in binary form are 16 bytes long

void memfill(uint8_t* byteArray)
{
  int size;// we will process 32 bit words generated by esp_random;
  for(size = 0; size < (UUID_LENGTH / 4); size++)
  {
    uint32_t randomWord = esp_random();
    uint8_t *bytes = (uint8_t*)&randomWord;

    int byteCount;
    for(byteCount=0; byteCount < 4; byteCount++)
    {
      *byteArray = *bytes;
      bytes++;
      byteArray++;
    }
  }

  // Add version number as 4
  byteArray[6] = 0x40 | (0x0F & byteArray[6]);
  byteArray[8] = 0x80 | (0x3F & byteArray[8]);
  
}
    char* hexToString(byte number)
  {
    int topDigit = number >> 4;
    int bottomDigit = number & 0x0f;
    char *charByte = (char*)malloc(2*sizeof(char));

    charByte[0] = "0123456789abcdef"[topDigit];
    charByte[1] = "0123456789abcdef"[bottomDigit];

    return charByte;
    }
    
  char * uuidv4()
  {
    int uuidLength = 36;
    char * uuidString = (char*)malloc((uuidLength+1)*sizeof(char));
    byte uuidNumber[UUID_LENGTH];
    memfill(uuidNumber);
    
    int byteCounter;
    for (byteCounter = 0; byteCounter < 16; byteCounter++) {
      if(byteCounter == 4 || byteCounter == 6 || byteCounter == 8 || byteCounter == 10)
      {
        *uuidString = '-';
        uuidString++;
        }
        char * charByte = hexToString(uuidNumber[byteCounter]);
        *uuidString = *charByte;
        uuidString++; charByte++;
        *uuidString = *charByte;

        uuidString++;
      }
      *uuidString = '\0';

      return uuidString - uuidLength;
  }
