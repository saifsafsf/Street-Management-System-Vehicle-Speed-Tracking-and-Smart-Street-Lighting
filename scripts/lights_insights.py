import pandas as pd
from collections import defaultdict
from generate_data import generate
import os

def lights_summary(filepath: str, num_of_observations=7200, save=False, save_path='data/lights_insights.csv') -> pd.DataFrame:
    '''
    Summarizes street lights data, the duration every light was on for

    Parameters
    ----------
    filepath : str
        filepath to previously generated data
    num_of_observations : int
        number of rows required to simulate the use case, required only if data is not found on ``filepath``
        (default 7200)
    save : bool
        flag to show if data sould be returned or saved as a .csv file, optional
        (default False)
    filepath : str
        path to the file to store the generated data, optional
        (default "data/lights_insights.csv")

    Returns
    -------
    pd.DataFrame or None (if save=True)
        lights column to represent the lights number
        hours column for the number of hours the corresponding light was on
    '''

    # if the data exists
    if os.path.exists(filepath):
        street_data = pd.read_csv(filepath)
    
    # else generate the data
    else:
        street_data = generate(num_of_observations)

    # cleaning " UTC" from the timestamp
    street_data['time'] = pd.to_datetime(street_data['time'].str[:-4])

    # for a dataframe-like structure
    lights_insights = defaultdict(list)

    for col in street_data.columns[:-2]:

        # for each light, calculate the time it was on for
        light_data = street_data[[col, 'time']]
        light_data = light_data[(light_data[[col]].shift() != light_data[[col]]).any(axis=1)]
        total_time_on = light_data.diff()[light_data.diff()[col] == -1]['time'].sum()

        # converting from seconds to hours
        total_hours = total_time_on.total_seconds() / 3600

        # adding to the dataframe
        lights_insights['lights'].append(col)
        lights_insights['hours'].append(round(total_hours, 2))
    
    lights_insights = pd.DataFrame(dict(lights_insights))

    # saving to disk or returning the data
    if save:
        lights_insights.to_csv(save_path, index=False)
    else: 
        return lights_insights


if __name__ == '__main__':
    lights_summary('data/street_data.csv', save=True)