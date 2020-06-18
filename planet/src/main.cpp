#include "verifierContext.hpp"
#include <iostream>
void printSMTLibInstance(bool includingApproximationConstraints);

//===== LI JIANLIN =====

//======================

//===== LI JIANLIN =====

#include <signal.h>
#include <fstream>

std::string boundFileName = "";
std::string summaryFileName = "";
std::string ans = "";
std::string msg = "";
std::string target_label ="";
bool SAT;
bool bound_only = false;
VerificationProblem problem;

void dump_statistics(const std::string& result){
    long long cnt = 0;
    bool can_proof = 1;
    int neuron_size = 0;

    if ( boundFileName != "") {
        auto bounds = problem.getInitialNeuronLimitBounds();
        auto names  = problem.getNodeNames();
        auto lookup = problem.getNodeNumberLookup();
        std::cout << "bound file name : " << boundFileName << std::endl;
        std::ofstream boundFile(boundFileName,std::ios::out);
        for (unsigned long i = 0; i < bounds.size(); ++i) {
            boundFile << names[i] << "\t"
                      << std::setprecision (std::numeric_limits<double>::digits10 + 1)
                      << bounds[i].first  << '\t'
                      << bounds[i].second << std::endl;
            if ( names[i].find("resX") == std::string::npos &&
                 names[i].find("inX")  == std::string::npos &&
                 names[i].find("outX")  == std::string::npos   ) {
                neuron_size ++;
                if ( ((bounds[i].first == 0) && (bounds[i].second == 0)) ||
                     ((bounds[i].first  > 0) && (bounds[i].second  > 0)) ){
                    cnt ++ ;
                    std::cout << "cnt++ " << names[i] << std::endl;
                    std::cout << std::setprecision (std::numeric_limits<double>::digits10 + 1)
                              << bounds[i].first  << '\t'
                              << bounds[i].second << std::endl;
                }

            }


            double label_low = bounds[lookup[target_label]].first;
            if ( ( names[i].find("outX") != std::string::npos ) &&
                 ( names[i] != target_label ) ) {
                if (bounds[i].second >= label_low) {
                    can_proof = false;
                    std::cout << "bad " << names[i] << std::endl;
                }
                else {
                    std::cout << "good " << names[i] << std::endl;
                }
            }
        }
        boundFile.close();
    }

    if(summaryFileName != "") {

        auto current = boost::posix_time::microsec_clock::local_time();
        auto totalTime = current - problem.startTime;

        long long initialTime = problem.initialWorkTime.total_milliseconds();
        long long solveTime   = problem.solveTime.total_milliseconds();
        long long total = totalTime.total_milliseconds() ;


        std::ofstream summary(summaryFileName,std::ios::app);
        summary << msg << ",\t"
                << result << ",\t "
                << totalTime << ",\t"
                << total << ",\t"
                << initialTime << ",\t"
                << solveTime << ",\t"
                << current << ",\t"
                << neuron_size << ",\t"
                << cnt <<",\t"
                << can_proof << std::endl;

        summary.close();
    }
}
void got_signal( int )
{
    printf( "Got signal\n" );
    dump_statistics("TIMEOUT");
    exit(0);
}
//======================


int main(int argc, char* argv[])
{

    //===== LI JIANLIN =====
    struct sigaction sa;
    memset( &sa, 0, sizeof(sa) );
    sa.sa_handler = got_signal;
    sigfillset( &sa.sa_mask );
    sigaction( SIGTERM, &sa, NULL );
    //======================

    try {
        //VerificationProblem problem;
        int printSMTLibInstance = 0;
        int printILPInstance = 0;
        std::string inFileName = "";
        for (unsigned int i=1;i<static_cast<unsigned int>(argc);i++) {
            if (std::string(argv[i]).substr(0,1)=="-") {
                // Parameters
                if (std::string(argv[i]) == "--smtlib") {
                    printSMTLibInstance = 1;
                } else if (std::string(argv[i]) == "--smtlibApprox") {
                    printSMTLibInstance = 2;
                } else if (std::string(argv[i]) == "--ilp") {
                    printILPInstance = 1;
                } else if (std::string(argv[i]) == "--ilpApprox") {
                    printILPInstance = 2;
                    //===== LI JIANLIN =====
                } else if (std::string(argv[i]) == "--summary") {
                    summaryFileName = std::string(argv[i + 1]);
                    i++;
                } else if (std::string(argv[i]) == "--msg") {
                    msg = std::string(argv[i + 1]);
                    i++;
                } else if (std::string(argv[i]) == "--bound") {
                    bound_only = true;
                    boundFileName = std::string(argv[i + 1]);
                    i++;
                } else if (std::string(argv[i]) == "--label") {
                    target_label = "outX" + std::string(argv[i + 1]);
                    i++;
                    //======================
                } else {
                    std::cerr << "Error: Did not understand parameter '" << argv[i] << "'\n";
                    return 1;
                }
            } else {
                if (inFileName=="") {
                    inFileName = argv[i];
                } else {
                    throw "Error: More than one file name given.";
                }
            }
        }
        problem.load(inFileName);

        if (printSMTLibInstance==1) { // No Approx
            problem.printSMTLibInstance(false);
        } else if (printILPInstance==1) {
            problem.printILPInstance(false);
        } else {
            if (!(problem.computeInitialNeuronLimitBounds())) {
                if (printSMTLibInstance>0)
                    std::cout << "(set-logic QF_LRA) (declare-fun a () Bool) (assert a) (assert (not a)) (check-sat)\n";
                else if (printILPInstance>0)
                    std::cout << "Subject To\n  x <= 0\n  x >= 1\nEnd\n";
                else
                    std::cout << "UNSAT\n";
                    dump_statistics("UNSAT");
            } else {
                if (printILPInstance>0) {
                    problem.printILPInstance(true);
                } else if (printSMTLibInstance>0) {
                    problem.printSMTLibInstance(true);
                } else {
                    if ( bound_only ) {
                        dump_statistics("BOUND");
                    }
                    else {
                        problem.verify();
                    }
                }
            }
        }

    } catch (const char *c) {
        std::cerr << "Error: " << c << std::endl;
        return 1;
    } catch (std::string c) {
        std::cerr << "Error: " << c << std::endl;
        return 1;
    }

}
