import pandas as pd
import numpy as np
np.random.seed(42)

def generate(num_of_observations):
    zero_cars_detected = num_of_observations // 3
    over_speed_cars = (num_of_observations - zero_cars_detected) // 5
    normal_speed_cars = over_speed_cars * 4

    df = pd.DataFrame(np.zeros((num_of_observations, 7)), columns=[f'lights_{i}' for i in range(1, 8)]).astype(int)

    df['speed'] = np.random.permutation(
        np.append(
            np.zeros(zero_cars_detected),
            np.append(
                np.random.uniform(low=30, high=81, size=(normal_speed_cars,)).round(2),
                np.random.uniform(low=81, high=121, size=(over_speed_cars,)).round(2)
            )
        )
    )

    df['time'] = pd.date_range('2023-12-24 08:00:00', periods=num_of_observations, freq='5s').astype(str) + ' UTC'

    lights_dict = {
        'below_60':[[0, 1, 2, 3], [2, 3, 4, 5], [4, 5, 6]],
        'below_90':[[0, 1, 2, 3, 4], [2, 3, 4, 5, 6]],
        'above_90':[[0, 1, 2, 3, 4, 5, 6]]
    }

    for idx, row in df.iterrows():
        if row['speed'] < 25:
            continue
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

    imp_columns = df.columns[:-1]
    df = df[(df[imp_columns].shift() != df[imp_columns]).any(axis=1)].reset_index(drop=True)

    