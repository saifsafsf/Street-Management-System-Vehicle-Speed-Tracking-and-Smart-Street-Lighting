import pandas as pd
import numpy as np
np.random.seed(42)

def generate(num_of_observations: int) -> pd.DataFrame:
    '''
    Simulates street light & speed detection system by generating specified number of rows of data

    Parameters
    ----------
    num_of_observations : int
        number of rows required to simulate the use case

    Returns
    -------
    pd.DataFrame
        separate column for each street light, (0 means off, 1 means on)
        speed column for detected speed (km/h)
        timestamp column
    '''

    # numbers of different types of observations
    empty_road = num_of_observations // 3
    over_speed_cars = (num_of_observations - empty_road) // 5
    normal_speed_cars = num_of_observations - empty_road - over_speed_cars

    # dataframe showing all street lights off (0 default value)
    df = pd.DataFrame(np.zeros((num_of_observations, 7)), columns=[f'lights_{i}' for i in range(1, 8)]).astype(int)

    # generating random speed detections (33% empty road, 13.3% overspeeding cars, 53.7% normal speed cars)
    df['speed'] = np.random.permutation(
        np.append(
            np.zeros(empty_road),
            np.append(
                np.random.uniform(low=30, high=81, size=(normal_speed_cars,)).round(2),
                np.random.uniform(low=81, high=121, size=(over_speed_cars,)).round(2)
            )
        )
    )

    # generating one row every 5 seconds, adding 'UTC' to copy ThingSpeak format
    df['time'] = pd.date_range('2023-12-24 08:00:00', periods=num_of_observations, freq='5s').astype(str) + ' UTC'

    # how car's speed affects lights turning on
    lights_dict = {
        'below_60':[[0, 1, 2, 3], [2, 3, 4, 5], [4, 5, 6]],
        'below_90':[[0, 1, 2, 3, 4], [2, 3, 4, 5, 6]],
        'above_90':[[0, 1, 2, 3, 4, 5, 6]]
    }

    for idx, row in df.iterrows():
        
        # if speed is zero, no car detected
        if row['speed'] < 25:
            continue
        
        # based on speed, turning the lights on
        elif row['speed'] < 60:
            for indices in lights_dict['below_60']:
                df.iloc[idx, indices] = 1
                if idx < num_of_observations - 1:
                    idx += 1
                else:
                    break

        elif row['speed'] < 90:
            for indices in lights_dict['below_90']:
                df.iloc[idx, indices] = 1
                if idx < num_of_observations - 1:
                    idx += 1
                else:
                    break

        else:
            for indices in lights_dict['above_90']:
                df.iloc[idx, indices] = 1

    # removing adjacent duplicates only (empty roads for longer than 5 seconds)
    imp_columns = df.columns[:-1]
    df = df[(df[imp_columns].shift() != df[imp_columns]).any(axis=1)].reset_index(drop=True)


if __name__ == '__main__':
    street_data = generate(7200)