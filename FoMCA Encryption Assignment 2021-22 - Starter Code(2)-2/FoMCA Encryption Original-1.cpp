// The text encryption program in C++ and ASM with a very simple example encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
//
// Original Author: A.Oram - 2012
// Last revised: A.Hamilton - Sep 2022


#include <string>     // for std::string
#include <chrono>     // for date & time functions
#include <ctime>      // for date & time string functions
#include <iostream>   // for std::cout <<
#include <fstream>    // for file I/O
#include <iomanip>    // for fancy output
#include <functional> // for std::reference_wrapper
#include <vector>     // for std::vector container


// *** KEEP ALL COMMENTS UP-TO-DATE AND TIDY, USEFULLY COMMENT ALL CODE YOU PRODUCE AND DELETE STALE COMMENTS (LIKE THIS ONE) ***
// *** THIS IS YOUR PROGRAM NOW, SO ALL COMMENTS ARE YOURS NOW! ***


constexpr char const * STUDENT_NAME = "Harris Fiaz"; // Replace with your full name
constexpr int ENCRYPTION_ROUTINE_ID = 16;                    // Replace -1 with your encryption id
constexpr char ENCRYPTION_KEY = 'a';                         // Replace '?' with your encryption key
constexpr int MAX_CHARS = 6;                                 // feel free to alter this, but must be 6 when submitting!

constexpr char STRING_TERMINATOR = '$';                      // custom string terminator
constexpr char LINE_FEED_CHARACTER = '\n';                   // line feed character (hhhmmm, this comment seems a bit unnecassary...)
constexpr char CARRIAGE_RETURN_CHARACTER = '\r';             // carriage return character

char original_chars[MAX_CHARS];                              // Original character string
char encrypted_chars[MAX_CHARS];                             // Encrypted character string
char decrypted_chars[MAX_CHARS] = "Soon!";                   // Decrypted character string, don't forget to delete default value when testing your decryption!


//---------------------------------------------------------------------------------------------------------------
//----------------- C++ FUNCTIONS -------------------------------------------------------------------------------

/// <summary>
/// get a single character from the user via Windows function _getwche
/// </summary>
/// <param name="a_character">the resultant character, pass by reference</param>
void get_char (char& a_character)
{
  a_character = (char)_getwche (); // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getche-getwche

  if (a_character == STRING_TERMINATOR) // skip further checks if user entered string terminating character
  {
    return;
  }
  if (a_character == LINE_FEED_CHARACTER || a_character == CARRIAGE_RETURN_CHARACTER)  // treat all 'new line' characters as terminating character
  {
    a_character = STRING_TERMINATOR;
    return;
  }
}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINE --------------------------------------------------------------------------

/// <summary>
/// 'encrypt' the characters in original_chars, up to length
/// output 'encrypted' characters in to encrypted_chars
/// </summary>
/// <param name="length">length of string to encrypt, pass by value</param>
/// <param name="EKey">encryption key to use during encryption, pass by value</param>
void encrypt_chars (int length, char EKey)
{
  char temp_char;                    // Temporary character store

  for (int i = 0; i < length; ++i)   // Encrypt characters one at a time
  {
    temp_char = original_chars [i];  // Get the next char from original_chars array
                                     // Note the lamentable lack of comments below!
    __asm
    {
      push   eax                     //
      push   ecx                     //
      push   edx                     //

      lea    eax, EKey               //
      movzx  ecx, temp_char          //
      call   encrypt_16              //  encrypts the character
      mov    temp_char, dl           //   to get the encrypted char we only need a small byte

      pop    edx                     //
      pop    ecx                     // gets the original values and restores them from the stack
      pop    eax                     //
    }

    encrypted_chars [i] = temp_char; // Store encrypted char in the encrypted_chars array
  }

  return;

  // Encrypt subroutine. You should paste in the encryption routine you've been allocated from BB and
  // overwrite this initial, simple, version. Ensure you change the �call� above to use the
  // correct 'encrypt_nn' label where nn is your encryption routine number.

  // Inputs: register EAX = 32-bit address of Ekey
  //                  ECX = the character to be encrypted (in the low 8-bit field, CL)
  // Output: register EDX = the encrypted value of the source character (in the low 8-bit field, DL)

  // REMEMBER TO UPDATE THESE COMMENTS AS YOU DO THE ASSIGNMENT. DELETE OLD/STALE COMMENTS.

  __asm
  {
  encrypt_16:
    push  ebx
    push  edx
    push  ecx
    movzx edx, byte ptr[eax]
    and   edx, 0x43
    cmp   edx, 0x00
    jnz   x16
    mov   edx, 0x07
x16:inc   dl
    mov   dword ptr[eax], edx
    pop   ebx
y16:dec   ebx
    dec   edx
    jnz   y16
    not   bl
    pop   edx
    mov   edx, ebx
    pop   ebx
    ret
  }
}
//*** end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINE --------------------------------------------------------------------------

/// <summary>
/// 'decrypt' the characters in encrypted_chars, up to length
/// output 'decrypted' characters in to decrypted_chars
/// </summary>
/// <param name="length">length of string to decrypt, pass by value</param>
/// <param name="EKey">encryption key to used during the encryption process, pass by value</param>
void decrypt_chars (int length, char EKey)
{
  /*** To be written by you ***/
}
//*** end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------





//************ MAIN *********************************************************************************************
//************ YOU DO NOT NEED TO EDIT ANYTHING BELOW THIS LINE *************************************************
//************ BUT FEEL FREE TO HAVE A LOOK *********************************************************************

void get_original_chars (int& length)
{
  length = 0;

  char next_char;
  do
  {
    next_char = 0;
    get_char (next_char);
    if (next_char != STRING_TERMINATOR)
    {
      original_chars [length++] = next_char;
    }
  }
  while ((length < MAX_CHARS) && (next_char != STRING_TERMINATOR));
}

std::string get_date ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  tm time_data;
  localtime_s (&time_data, &now);
  std::strftime (buf, sizeof(buf), "%d/%m/%Y", &time_data);
  return std::string{ buf };
}

std::string get_time ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  tm time_data;
  localtime_s (&time_data, &now);
  std::strftime (buf, sizeof (buf), "%H:%M:%S", &time_data);
  return std::string{ buf };
}

// support class to help output to multiple streams at the same time
struct multi_outstream
{
  void add_stream (std::ostream& stream)
  {
    streams.push_back (stream);
  }

  template <class T>
  multi_outstream& operator<<(const T& data)
  {
    for (auto& stream : streams)
    {
      stream.get () << data;
    }
    return *this;
  }

private:
  std::vector <std::reference_wrapper <std::ostream>> streams;
};

int main ()
{
  int char_count = 0;  // The number of actual characters entered (upto MAX_CHARS limit)

  std::cout << "Please enter upto " << MAX_CHARS << " alphabetic characters: ";
  get_original_chars (char_count);	// Input the character string to be encrypted


  //*****************************************************
  // Open a file to store results (you can view and edit this file in Visual Studio)

  std::ofstream file_stream;
  file_stream.open ("log.txt", std::ios::app);
  file_stream << "Date: " << get_date () << " Time: " << get_time () << "\n";
  file_stream << "Name:                  " << STUDENT_NAME << "\n";
  file_stream << "Encryption Routine ID: '" << ENCRYPTION_ROUTINE_ID << "'" << "\n";
  file_stream << "Encryption Key:        '" << ENCRYPTION_KEY;

  multi_outstream output;
  output.add_stream (file_stream);
  output.add_stream (std::cout);


  //*****************************************************
  // Display and save to the log file the string just input

  output << "\n\nOriginal string  = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << original_chars;

  // output each original char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const original_char = static_cast <int> (original_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << original_char << " ";
  }


  //*****************************************************
  // Encrypt the string and display/save the result

  encrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nEncrypted string = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << encrypted_chars;

  // output each encrypted char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const encrypted_char = static_cast <int> (encrypted_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << encrypted_char << " ";
  }


  //*****************************************************
  // Decrypt the encrypted string and display/save the result

  decrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nDecrypted string = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << decrypted_chars;

  // output each decrypted char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const decrypted_char = static_cast <int> (decrypted_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << decrypted_char << " ";
  }


  //*****************************************************
  // End program

  output << "\n\n";
  file_stream << "-------------------------------------------------------------\n\n";
  file_stream.close ();

  system ("PAUSE"); // do not use in your other programs! just a hack to pause the program before exiting

  return 0;
}

//**************************************************************************************************************
