// This file contains the necessary functions for producing Morse Code on an LED either onboard or off.
// You will need to define the pin for the LED

// By Josh Hale "sn0wfa11"

int dot_length = 100;
int dash_length = dot_length * 3;
int pause_element = dot_length;
int pause_char = dot_length * 2; // standard is *3 (one after last element, then two more)
int pause_word = dot_length * 4; // standard is *7 (one after last element, four at end of word, then two more at end of char)

// This function will produce a dot on the led
//
// @return [void] A useful return value is not expected
void led_dot()
{
  digitalWrite(led, HIGH);
  delay(dot_length);
  digitalWrite(led, LOW);
  delay(pause_element);
}

// This function will produce a dash on the led
//
// @return [void] A useful return value is not expected
void led_dash()
{
  digitalWrite(led, HIGH);
  delay(dash_length);
  digitalWrite(led, LOW);
  delay(pause_element);
}

// ------------------------------------------------------------------------------------------------------------------------------------
// This function will output the string provided as morse code on the LED
//
// @string [string] Input string to be displayed as morse code on the LED
//
// @return [void] A useful return value is not expected
void morse_code(char string[])
{
  int count, length;
  length = strlen(string);
  for(count = 0 ; count < length ; count++)
  {
    char chr = string[count];
    blink_code(code_string(chr));
    delay(pause_char);
  }
}

// This function will convert a char to morse code
// H = Dash, and D = Dot
//
// @in [char] input char
//
// @return [string] string of dashes or dot to be displayed on LED
char * code_string(char in)
{
  if ((in == 'a') || (in == 'A')) return "DH";
  else if ((in == 'b') || (in == 'B')) return "HDDD";
  else if ((in == 'c') || (in == 'C')) return "HDHD";
  else if ((in == 'd') || (in == 'D')) return "HDD";
  else if ((in == 'e') || (in == 'E')) return "D";
  else if ((in == 'f') || (in == 'F')) return "DDHD";
  else if ((in == 'g') || (in == 'G')) return "HHD";
  else if ((in == 'h') || (in == 'H')) return "DDDD";
  else if ((in == 'i') || (in == 'I')) return "DD";
  else if ((in == 'j') || (in == 'J')) return "DHHH";
  else if ((in == 'k') || (in == 'K')) return "HDH";
  else if ((in == 'l') || (in == 'L')) return "DHDD";
  else if ((in == 'm') || (in == 'M')) return "HH";
  else if ((in == 'n') || (in == 'N')) return "HD";
  else if ((in == 'o') || (in == 'O')) return "HHH";
  else if ((in == 'p') || (in == 'P')) return "DHHD";
  else if ((in == 'q') || (in == 'Q')) return "HHDH";
  else if ((in == 'r') || (in == 'R')) return "DHD";
  else if ((in == 's') || (in == 'S')) return "DDD";
  else if ((in == 't') || (in == 'T')) return "H";
  else if ((in == 'u') || (in == 'U')) return "DDH";
  else if ((in == 'v') || (in == 'V')) return "DDDH";
  else if ((in == 'w') || (in == 'W')) return "DHH";
  else if ((in == 'x') || (in == 'X')) return "HDDH";
  else if ((in == 'y') || (in == 'Y')) return "HDHH";
  else if ((in == 'z') || (in == 'Z')) return "HHDD";
  else if (in == '1') return "DHHHH";
  else if (in == '2') return "DDHHH";
  else if (in == '3') return "DDDHH";
  else if (in == '4') return "DDDDH";
  else if (in == '5') return "DDDDD";
  else if (in == '6') return "HDDDD";
  else if (in == '7') return "HHDDD";
  else if (in == '8') return "HHHDD";
  else if (in == '9') return "HHHHD";
  else if (in == '0') return "HHHHH";
  else
    return " ";
}

// This function will produce a dash or dot depending on the input
//
// @code [string] Input string to convert to light dashes or dots
//
// @return [void] A useful return value is not expected
void blink_code(char code[])
{
int count, length;
  length = strlen(code);
  for(count = 0 ; count < length ; count++)
  {
    char chr = code[count];
    if (chr == 'D')
      led_dot();
    else if (chr == 'H')
      led_dash();
    else
      delay(pause_word);
  }
}
