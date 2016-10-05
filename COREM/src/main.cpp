
/* BeginDocumentation
 * Name: main
 *
 * Description: fixed time-step simulation of the retina script
 * passed through arguments.
 *
 *
 * Author: Pablo Martinez Cañada. University of Granada. CITIC-UGR. Spain.
 * <pablomc@ugr.es>
 *
 * SeeAlso:InterfaceNEST
 */

#include <dirent.h>
#include "../CImg-1.6.0_rolling141127/CImg.h"
#include "InterfaceNEST.h"
#include "constants.h"

using namespace cimg_library;
using namespace std;


// main
int main(int argc, char *argv[])
{

    string currentDirRoot = constants::getPath();

    // delete files in results folder (if any)
    DIR *dir;
    struct dirent *ent;
    string resdir = currentDirRoot+"results/";
    const char * charesdir = (resdir).c_str();

    if ((dir = opendir (charesdir)) != NULL) {
        int files = 0;
        while ((ent = readdir (dir)) != NULL) {
            files+=1;
          }

        if(files > 2){
            string results = "exec rm "+currentDirRoot+"results/*";
            const char * todelete = (results).c_str();
            system(todelete);
        }
        closedir (dir);
    }else{
        string tocreate = "mkdir "+currentDirRoot+"results/";
        system(tocreate.c_str());
    }

    // Create retina interface
    string retinaString;
    const char * outID;
    int arg_index;
    bool got_script_file;
    bool verbose_flag, help_param;

    // Default parameter values
    verbose_flag=false;
    help_param=false;
    got_script_file=false;
    // read arguments or default script
    for(arg_index=1;arg_index<argc;arg_index++){ // Parse all input arguments
        if(argv[arg_index][0]!='-'){ // If first character of argument is not '-', assume that it is the script filename
            if(!got_script_file){
                retinaString = currentDirRoot + (string)argv[arg_index];
                got_script_file=true;
            }else
                cout << "Warning: More than one argument seem retina script filenames: Using the first one:" << retinaString << endl;
        }
        if(strcmp(argv[arg_index],"-h") == 0 || strcmp(argv[arg_index],"--help") == 0 || strcmp(argv[arg_index],"/?") == 0){ // Help argument found
            cout << "COREM retina simulator." << endl;
            cout << " Syntax: " << argv[0] << " [-v] <retina_script_filename>" << endl;
            cout << "   <retina_script_filename> is a text file (usually with extension .py) which" << endl;
            cout << "   defines a retina model and simulation parameters." << endl;
            cout << "   Visit https://github.com/pablomc88/COREM/wiki for information about the" << endl;
            cout << "   format of this script file" << endl;
            help_param=true;
        }
        
        if(strcmp(argv[arg_index],"-v") == 0) // Verbose execution requested
            verbose_flag=true;
    }
    if(got_script_file){
        // Create interface
        const char * retinaSim = retinaString.c_str();
        InterfaceNEST interface;
        interface.allocateValues(retinaSim,"output",constants::outputfactor,0);

        // Read number of trials and simulation time
        double trials = interface.getTotalNumberTrials();
        int simTime = interface.getSimTime();
        double simStep = interface.getSimStep();

        if(verbose_flag){
            cout << "Simulation time: "<< simTime << endl;
            cout << "Trials: "<< trials << endl;
            cout << "Simulation step: "<< simStep << endl;
        }

        // Simulation
        for(int i=0;i<trials;i++){

            // Create new retina interface for every trial (reset values)
            InterfaceNEST interface;
            interface.setVerbosity(verbose_flag);
            interface.allocateValues(retinaSim,"output",constants::outputfactor,i);

            if(verbose_flag){
                cout << "-- Trial "<< i << " --" << endl;
                cout << "   AbortExecution " << interface.getAbortExecution() << endl;
            }

            if(interface.getAbortExecution()==false){
                for(int k=0;k<simTime;k+=simStep){
                    interface.update();
                }
            }

        }
        
    }else{
        if(!help_param){
            cout << "Please provide a retina script filename in arguments" << endl;
            cout << "Execute '" << argv[0] << " -h' for more help" << endl;
        }
    }    
   return 1;
}
