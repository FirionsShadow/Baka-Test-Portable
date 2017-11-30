#include <iostream>
#include <fstream>
#include <string>
#include <limits>

#define NUMBER_OF_SCRIPTS 85
#define SCRIPT_OFFSET 1331200
#define SECTOR_SIZE 2048
#define HALFWORD 2

using namespace std;

std::streamsize fileSize( const char* filePath );

int main(int argc, char *argv[])
{

  if ( argc != 4 ) // argc should be 2 for correct execution
    // We print argv[0] assuming it is the program name
    cout<<"usage: "<< argv[0] <<" <eboot file> <script directory> <output filename>\n";
  else {
    // We assume argv[1] is a filename to open
    ifstream eboot(argv[1], ios::binary);
    // Always check to see if file opening succeeded
    if (!eboot.is_open()) {
      cout << "Could not open " << argv[1] << '\n';
      return 1;
    }

    fstream output(argv[3], std::ios_base::binary | std::ios_base::out);
    if (!output.is_open()) {
        cout << "Could not open " << argv[3] << '\n';
        return 3;
    }
    else {
        // Copy source to destination file
        output << eboot.rdbuf();
        eboot.close();

        // This is equal to the combined size of each previous file
        int cumulativeOffset = 0;

        // Move position to the start of the script file lookup table
        output.seekp(SCRIPT_OFFSET, std::ios_base::beg);

        for(int i = 0; i < NUMBER_OF_SCRIPTS; i++) {
            // get name of file, should be labeled 0 - 84
            string filepath = argv[2] + std::to_string(i);

            int size = fileSize(filepath.c_str());

            // File size needs to be divided by 2048 to get the number of sectors
            int sectorCount = size / SECTOR_SIZE;

            //cout << "File " << i << ": " << size << " bytes\n";

            // Write the script file's entry to the lookup table
            output.write(reinterpret_cast<const char *>(&cumulativeOffset), HALFWORD);
            output.write(reinterpret_cast<const char *>(&sectorCount), HALFWORD);

            cumulativeOffset += sectorCount;
        }

        // Close output file and prevent truncation
        output.close();
        output.open(argv[3], std::ios_base::binary | std::ios_base::out | std::ios_base::in);

        // Print confirmation message
        cout << "Successfully created " << argv[3] << '\n';
    }
  }
  return 0;
}

std::streamsize fileSize( const char* filePath ){

    ifstream file(filePath, std::ios::binary);
    file.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize length = file.gcount();
    file.clear();   //  Since ignore will have set eof.
    file.seekg( 0, std::ios_base::beg );

    return length;
}
