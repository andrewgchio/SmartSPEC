# Contains smoothening functions to apply to columns of dataframes

import pandas as pd

def simple_moving_avg(df, window, data_col):
    '''
    Performs simple moving average smoothening on a time series

    :param df: A pandas dataframe with at least 2 columns:
            'time': Column storing starting time
            '{data_col}': Column storing time series data
    :param window: An integer representing window size to perform SMA
    :param data_col: Name of column storing time series data
    :return: time series df with extra column 'SMA_{window}'
    '''
    col = f'SMA_{window}'
    if col in df: 
        return df, col
    for i in range(window):
        df.loc[df.index[i], col] = df.loc[df.index[i], data_col]
    for i in range(df.shape[0] - window + 1):
        df.loc[df.index[i+window-1], col] = \
                np.round(sum(df.loc[df.index[i+k], data_col] \
                    for k in range(window)) / window, 1)
    return df, col

def exponential_moving_avg(df, span, data_col):
    '''
    Perform exponential moving average smoothening on a time series.

    :param df: A pandas dataframe with at least 2 columns:
            'time': Column storing starting time
            '{data_col}': Column storing time series data.
    :param span: Integer. Roughly corresponds to the window of a moving 
            average; it controls how fast the weights drop off, so it 
            determines the number of points that make a non-negligible 
            contribution to each average.
    :param data_col: Name of column storing time series data.
    :return: time series df with extra column 'EMA_SPAN_{span}'.
    '''
    col = f'EMA_SPAN_{span}'
    if col in df:
        return df, col
    df[col] = df.loc[:, data_col].ewm(span=span, adjust=False).mean()
    df.round({col: 1})
    return df, col

