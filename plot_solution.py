#!/usr/bin/env python

import argparse
import os

import numpy as np
import matplotlib
from matplotlib import pyplot as plt
import pandas as pd


if __name__ == '__main__':

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('input_file', help='Input CSV file.')
    arg_parser.add_argument(
        '-i', '--index', type=int, default=0,
        help='Index of a solution from the input file.')
    arg_parser.add_argument(
        '--families', default='family_data.csv',
        help='CSV file with family data.')
    arg_parser.add_argument(
        '-o', '--output', default='fig', help='Output directory for figures.')
    args = arg_parser.parse_args()

    try:
        os.makedirs(args.output)
    except FileExistsError:
        pass

    NUM_DAYS = 100
    NUM_CHOICES = 10

    with open(args.input_file) as f:
        cur_index = 0
        assignment = None
        for line in f:
            if cur_index == args.index:
                assignment = np.asarray([int(w) for w in line.split(',')])
                break
            cur_index += 1

    family_data = pd.read_csv(args.families)
    family_sizes = family_data['n_people'].to_numpy()
    preferences = family_data[
        ['choice_{}'.format(i) for i in range(NUM_CHOICES)]].to_numpy()

    day_occupancy = np.zeros((NUM_DAYS, NUM_CHOICES + 1), dtype=int)
    choice_occupancy = np.zeros(NUM_CHOICES + 1, dtype=int)
    for family, day in enumerate(assignment):
        indices = np.argwhere(preferences[family] == day)
        if indices.size > 0:
            choice = indices[0, 0]
        else:
            choice = NUM_CHOICES
        day_occupancy[day - 1, choice] += family_sizes[family]
        choice_occupancy[choice] += family_sizes[family]

    fig = plt.figure()
    axes = fig.add_subplot()
    cmap = matplotlib.cm.get_cmap('plasma')
    axes.hist(
        [list(range(1, NUM_DAYS + 1))] * (NUM_CHOICES + 1),
        bins=list(range(1, NUM_DAYS + 2)), weights=day_occupancy,
        color=[cmap(i / NUM_CHOICES) for i in range(NUM_CHOICES + 1)],
        histtype='stepfilled', stacked=True)
    axes.margins(x=0)
    for n in [125, 300]:
        axes.axhline(n, ls='dashed', lw=0.8, c='black')
    axes.set_xlabel('Day')
    axes.set_ylabel('Occupancy')
    fig.savefig(os.path.join(args.output, 'occupancy.pdf'))
    plt.close(fig)

    fig = plt.figure()
    axes = fig.add_subplot()
    axes.bar(range(NUM_CHOICES + 1), choice_occupancy)
    axes.set_yscale('log')
    axes.set_xlabel('Choice')
    axes.set_ylabel('Number of people')
    fig.savefig(os.path.join(args.output, 'choices.pdf'))
    plt.close(fig)
