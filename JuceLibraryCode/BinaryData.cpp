/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== README.md ==================
static const unsigned char temp_binary_data_0[] =
"# PadMan\n"
"This is a useful tool for learning how to play Ableton Push, LaunchPad, APC64, Linnstrument, and other pad controllers. PadMan tells you the degree of each note in a chord.  \n"
"https://twitter.com/watchan/status/1733245224942817651\n"
"\n"
"\n"
"\n"
"# Release Note\n"
"\n"
"## v1.0.2\n"
"- Bugfix: Fixed a bug where PadMan was not recognized as a VST plugin by some DAWs.  \n"
"- New feature: Implemented a feature that detects and displays the chord based on MIDI input.\n"
"\n"
"\n"
"## v1.0.1\n"
"Bugfix: Some DAWs had been recognize Padman as Audio Plug-in  \n"
"\n"
"\n"
"## v1.0.0\n"
"You can download PadMan_v1.0.0.zip from Release folder.  \n"
"This zip has PadMan as a MIDI plugin (Standalone App for mac / AUv2 / VST3)  \n";

const char* README_md = (const char*) temp_binary_data_0;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x64791dc8:  numBytes = 671; return README_md;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "README_md"
};

const char* originalFilenames[] =
{
    "README.md"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
        if (strcmp (namedResourceList[i], resourceNameUTF8) == 0)
            return originalFilenames[i];

    return nullptr;
}

}
