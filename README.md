# Kaggle competition: Santa's workshop tour 2019

Code for [2019 Christmas competition](https://www.kaggle.com/c/santa-workshop-tour-2019/overview) at Kaggle. A generalized assignment problem is tacked using a genetic algorithm. The source code is somewhat messy.


## Details of the genetic algorithm

A simple genotype consisting of days assigned for each family is used.

Individuals for reproduction are chosen with the tournament selection. A pair of parents produces two children. Randomly, the children either undergo a uniform cross-over or kept unchanged (in which case they are exact copies of the parents).

The children are subject to different types of mutations. Those include reassigning a family to a different day and different simultaneous reassignments of two families. The latter was proven crucial to respect the hard occupancy constraints.

New individuals that do not respect the hard constraints or that are identical to another living individual are rejected immediately and do not enter the new generation. The breeding continues until the size of the new generation becomes equal to the size of the old one.

At the survival phase, the old and the new generation are merged together, and a half of individuals are selected. The selection favours individuals with smaller loss, and the best individual is always selected (elitism).

Once every few generations a brute force search is performed in the vicinity of the solutions found so far.


## Performance

The best score obtained with this code was around 75k (top 54%), taking around a day of CPU time in total. One possible improvement would be to replace the hard occupancy constraints with soft ones and ‘anneal’ them over time.
