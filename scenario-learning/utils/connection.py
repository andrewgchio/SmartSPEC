import mysql.connector as myc

# A context manager that connects to the MySQL database using the given login 
# info. Use as a context manager, e.g., 
#
#       with Connection() as conn: 
#           ...
#
class Connection:

    def __init__(self):
        '''
        Credentials for MySQL client, server 
        '''
        self.user = input('Enter user: ')
        self.passwd = getpass('Enter pass: ')

    def _login(self):
        '''
        Return a MySQL connection, using the credentials from the given file
        :param fname: a file containing credential information
        '''
        config = {
            'ssl_disabled' : True,
            'user' : self.user,
            'password' : self.passwd,
            'database' : 'simulation_seed',
            'host' : 'localhost',
            'port' : '3306'
        }

        return myc.connect(**config)

    def __enter__(self):
        '''
        Setup management for the context manager
        '''
        self.lcnx = self._login()
        return self

    def __exit__(self, exc, exc_value, traceback):
        '''
        Teardown management for the context manager
        '''
        self.lcnx.close()
        return False
