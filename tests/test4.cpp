// TEST Nelson-Siegel
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <iomanip> 
#include <filesystem>
#include "../include/cpp-quant/tools/nss.hpp"

// Zero yields data of Bank of Canada as of September 24th, 2025 (https://www.bankofcanada.ca/rates/interest-rates/bond-yield-curves/)
std::vector<double> DATA = {
    0.0242277000,0.0240667000,0.0239455000,0.0239879000,	
    0.0242138000,0.0242958000,0.0244047000,0.0245381000,	
    0.0246938000,0.0248696000,0.0250637000,0.0252740000,
    0.0254988000,0.0257363000,0.0259849000,0.0262429000,	
    0.0265089000,0.0267814000,0.0270591000,0.0273407000,	
    0.0276251000,0.0279111000,0.0281979000,0.0284843000,
    0.0287696000,0.0290529000,0.0293336000,0.0296110000,	
    0.0298844000,0.0301535000,0.0304176000,0.0306763000,	
    0.0309294000,0.0311764000,0.0314171000,0.0316513000,
    0.0318788000,0.0320994000,0.0323131000,0.0325197000,	
    0.0327192000,0.0329116000,0.0330970000,0.0332752000,
    0.0334465000,0.0336109000,0.0337684000,0.0339193000,
    0.0340636000,0.0342015000,0.0343332000,0.0344587000,
    0.0345784000,0.0346924000,0.0348009000,0.0349041000,
    0.0350022000,0.0350955000,0.0351840000,0.0352681000,
    0.0353480000,0.0354238000,0.0354958000,0.0355642000,
    0.0356293000,0.0356911000,0.0357499000,0.0358059000,
    0.0358593000,0.0359103000,0.0359590000,0.0360057000,
    0.0360504000,0.0360934000,0.0361347000,0.0361746000,
    0.0362132000,0.0362506000,0.0362869000,0.0363222000,
    0.0363567000,0.0363904000,0.0364235000,0.0364560000,
    0.0364880000,0.0365196000,0.0365508000,0.0365818000,
    0.0366124000,0.0366429000,0.0366732000,0.0367033000,
    0.0367334000,0.0367633000,0.0367932000,0.0368230000,
    0.0368527000,0.0368823000,0.0369119000,0.0369414000,
    0.0369708000,0.0370000000,0.0370291000,0.0370580000,
    0.0370868000,0.0371152000,0.0371434000,0.0371713000,
    0.0371988000,0.0372260000,0.0372526000,0.0372787000,
    0.0373043000,0.0373293000,0.0373535000,0.0373771000,
    0.0373998000,0.0374217000,0.0374427000,0.0374627000
};

std::map<double, double> getYearFractionMappedData()
{
    std::map<double, double> data; 
    double t = 0; 
    for (const double y: DATA){t+=.25;data[t] = y;}
    return data;
}

void writeNelsonSiegelResult(const std::shared_ptr<NelsonSiegelFamily>& ns, std::string fileName) {

    // Open file for writing
    std::filesystem::create_directory("output-test4");
    std::ofstream file("output-test4/" + fileName +".csv");
    if (!file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
    }

    // Write column headers
    file << "timeToMaturity,observedSpotRate,nssSpotRate,nssForwardRate,nssDerivativeForwardRate, \n";

    std::map<double, double> data = getYearFractionMappedData();

    for (const auto& d:data){
        file << d.first << "," << d.second << "," << ns->getRate(d.first) << "," << ns->getInstantaneousForwardRate(d.first) << "," << ns->getDerivativeInstantaneousForwardRate(d.first) << "," <<"\n";
    }

    file.close();
    std::cout << "Nelson-siegel curve written successfully!" << std::endl;

}

void testOLS()
{
    NelsonSiegelCalibration nsCalib(getYearFractionMappedData(), true, false);
    NelsonSiegelCalibration svenssonCalib(getYearFractionMappedData(), true, true);

    double tau1 = 2.0; 
    double tau2 = 10.0;

    writeNelsonSiegelResult(nsCalib.getOLS(tau1,tau2), "nelsonSiegelCurveOLS"); 
    writeNelsonSiegelResult(svenssonCalib.getOLS(tau1,tau2), "svenssonCurveOLS"); 

}

void testNelderMead()
{
    NelsonSiegelCalibration nsCalib(getYearFractionMappedData(), true, false);
    NelsonSiegelCalibration svenssonCalib(getYearFractionMappedData(), true, true);

    std::vector<double> nsParams = nsCalib.getNelderMeadObject().getResult();
    std::vector<double> svenssonParams = svenssonCalib.getNelderMeadObject().getResult();

    writeNelsonSiegelResult(nsCalib.getOLS(nsParams[0],10.0), "nelsonSiegelCurveNM"); 
    writeNelsonSiegelResult(svenssonCalib.getOLS(svenssonParams[0],svenssonParams[1]), "svenssonCurveNM"); 
}


int main()
{
    testOLS();
    testNelderMead();
    return 0;
}