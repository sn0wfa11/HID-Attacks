##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'rex'

class MetasploitModule < Msf::Post

  include Msf::Post::Windows::Powershell
  include Msf::Post::File
  include Msf::Post::Windows::Registry

  def initialize(info={})
    super( update_info( info,
      'Name'          => 'Windows Post Provide feedback for Teensy USB Device.',
      'Description'   => %q{
        This module will press the selected key as feedback to a Teensy USB Device.
        It will try to send the feedback using a vbscript first. If this does not succeed it will
        try to send the feedback as a powershell command.
      },
      'License'       => MSF_LICENSE,
      'Author'        => [ 'Josh Hale "sn0wfa11"'],
      'Platform'      => 'win',
      'SessionTypes'  => [ 'meterpreter' ],
      'References'    =>
        [
          ['URL', 'https://github.com/jhale85446/HID-Attacks']
        ]
    ))

    register_options(
      [
         OptInt.new('KEY', [ true, 'Key to press for feedback 1=NumLock, 2=CapsLock, 3=ScrollLock', 1, [1,2,3]])
      ], self.class)
  end

  def run
    if datastore['KEY'] == 1
      key = "NUMLOCK"
    elsif datastore['KEY'] == 2
      key = "CAPSLOCK"
    else
      key = "SCROLLLOCK"
    end

    print_status("Providing feedback for Teensy.")

    if vbscript_feedback(key)
      print_good("Feedback sent!")
    else
      if powershell_feedback(key)
        print_good("Feedback sent!")
      else
        print_error("Unable to send feedback!")
      end
    end
  end

  # This function will execute a powershell command that presses the selected key.
  #
  # @return [TrueClass] If the feedback was sent.
  # @return [FalseClass] If the feedback was unable to be sent.
  def powershell_feedback(key)
    begin
      psh_exec("$keyboard = New-Object -ComObject WScript.Shell;$keyboard.SendKeys('{#{key}}');")
      return true
    rescue ::Rex::Post::Meterpreter::RequestError => error
      print_error(error.to_s)
      return false
    end
  end

  # This function will make a vbscript on the remote computer that presses the selected key.
  # It will execute the vbscript then cleanup after itself.
  #
  # @return [TrueClass] If the feedback was sent.
  # @return [FalseClass] If the feedback was unable to be sent.
  def vbscript_feedback(key)
    homepath = get_homepath

    if homepath
      begin
        cmd_exec("cmd /c echo WScript.CreateObject(\"WScript.Shell\").SendKeys \"{#{key}}\" > #{homepath}z.vbs")
        cmd_exec("cmd /c cscript //nologo #{homepath}z.vbs")
        sleep 1
        delete_file(homepath + "z.vbs")
        return true
      rescue ::Rex::Post::Meterpreter::RequestError => error
        print_error(error.to_s)
        return false
      end
    else
      return false
    end    
  end

  # This function will return the current user's homepath.
  #
  # @return User's home path if the user is not a system account
  # @return nil if the user is a system account
  def get_homepath
    sid = client.sys.config.getsid
    profile_path = registry_getvaldata("HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\#{sid}","ProfileImagePath")
    
    if !profile_path.include?("Users")
      print_status("\tSID: #{sid} is not a normal user. Skipping.")
      return nil
    end

    profile_path = profile_path.strip
    profile_path = profile_path.gsub('\\', '/')

    return "#{profile_path}/"
  end

  # This function will delete a file.
  #
  # @return [TrueClass] If the file was deleted or not present to begin with
  # @return [FalseClass] If the file unable to be deleted.
  def delete_file(file)
    begin
      if exist?(file)
        session.fs.file.delete(file)
        if !exist?(file)
          return true         
        else
          print_error("\tUnable to remove file: #{file}")
          return false
        end      
      else
        return true
      end
    rescue ::Rex::Post::Meterpreter::RequestError => error
      print_error("Unable to remove file: #{file}.")
      print_error(error.to_s)
      return false
    end
  end

end
