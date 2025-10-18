from typing import List
import csv
import matplotlib.pyplot as plt 

class nelsonSiegelCurveFit: 

    def __init__(self, name:str):
        self.name = name 
        self.data = self.loadData() 
    
    def loadData(self) -> dict[str, List[float]]:
        timeToMat, obsSpotRate, obsDerivSpotRate, nssSpotRate, nssForwardRate, nssDerivativeForwardRate = [],[], [], [], [], []
        with open('build/output-test3/'+self.name+'.csv', newline="") as csvfile:
            reader = csv.DictReader(csvfile)  # Reads by column names
            for row in reader:
                timeToMat.append(float(row['timeToMaturity']))
                obsSpotRate.append(float(row['observedSpotRate']))
                obsDerivSpotRate.append(float(row['observedDerivativeSpotRate']))
                nssSpotRate.append(float(row['nssSpotRate']))
                nssForwardRate.append(float(row['nssForwardRate']))
                nssDerivativeForwardRate.append(float(row['nssDerivativeForwardRate']))
        return {
            'timeToMat':timeToMat, 
            'obsSpotRate' : obsSpotRate,
            'obsDerivSpotRate' : obsDerivSpotRate,
            'nssSpotRate' : nssSpotRate,
            'nssForwardRate' : nssForwardRate,
            'nssDerivativeForwardRate' : nssDerivativeForwardRate,
        }
    
    def plotFit(self, save: bool = False):
        fig = plt.figure(linewidth=1, figsize=(12, 9))
        plt.plot(self.data['timeToMat'], self.data['obsSpotRate'], 'ro', label = 'Observed rates', markersize = 1)
        plt.plot(self.data['timeToMat'], self.data['nssSpotRate'], color ='navy', label = 'Nelson siegel (Svensson) spot rate')
        plt.plot(self.data['timeToMat'], self.data['nssForwardRate'], color = 'skyblue', label = 'Nelson siegel (Svensson) forward rate')
        plt.xlabel('Time to maturity')
        plt.ylabel('Yield')
        plt.legend()
        if save: plt.savefig('build/output-test3/'+self.name+'-fit.png')
        plt.show()

nelsonSiegelCurveFit('canadaZeroCurveSvensson').plotFit()
nelsonSiegelCurveFit('canadaZeroCurveNelsonSiegel').plotFit()

