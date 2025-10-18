from typing import List
import csv
import matplotlib.pyplot as plt 

class Curve: 
    def __init__(self, name:str):
        self.name = name 
        self.data = self.loadData() 
    
    def loadData(self) -> dict[str, List[float]]:
        timeToMat, contRate, simpleRate, forwRate, derivForwRate = [], [], [], [], []
        with open('build/output-test4/'+self.name+'.csv', newline="") as csvfile:
            reader = csv.DictReader(csvfile)  # Reads by column names
            for row in reader:
                timeToMat.append(float(row['timeToMaturity']))
                contRate.append(float(row['continuousRate']))
                simpleRate.append(float(row['simpleRate']))
                forwRate.append(float(row['instantaneousForwardRate']))
                derivForwRate.append(float(row['derivativeInstantaneousForwardRate']))
        return {
            'timeToMat':timeToMat, 
            'contRate' : contRate,
            'simpleRate' : simpleRate,
            'forwRate' : forwRate,
            'derivForwRate' : derivForwRate,
        }
    
    def plotCurve(self, save: bool = True):
        fig = plt.figure(linewidth=1, figsize=(12, 9))
        plt.plot(self.data['timeToMat'], self.data['simpleRate'], label = 'Simple rate')
        plt.plot(self.data['timeToMat'], self.data['contRate'], label = 'Continuous rate')
        plt.plot(self.data['timeToMat'], self.data['forwRate'], label = 'Instantaneous forward rate')
        #plt.plot(timeToMat, derivForwRate, label = 'Derivative instantaneous forward rate')
        plt.xlabel('Time to maturity')
        plt.ylabel('Yield')
        plt.legend()
        if save: plt.savefig('build/output-test4/'+self.name+'.png')
        plt.show()

    def plotComparaisonCurve(self, otherCurve: 'Curve', toCompare:str = 'simpleRate', save: bool = True) -> None:
        fig = plt.figure(linewidth=1, figsize=(12, 9))
        plt.plot(self.data['timeToMat'], self.data[toCompare], 'ro', label = self.name, markersize = 1)
        plt.plot(self.data['timeToMat'], otherCurve.data[toCompare], label = otherCurve.name, color = 'navy')
        plt.xlabel('Time to maturity')
        plt.ylabel('Yield')
        plt.legend()
        if save: plt.savefig('build/output-test4/'+self.name+'-'+otherCurve.name+'-'+ toCompare +'.png')
        plt.show() 

curve1 = Curve('initialCanadianCurve')
curve2 = Curve('svenssonCanadianCurve')
curve2.plotCurve(False)
curve1.plotCurve(False)
curve2.plotComparaisonCurve(curve1,'simpleRate',False)
curve2.plotComparaisonCurve(curve1,'forwRate',False)



