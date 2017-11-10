"""Launches a basic webapp for cyclus simulations."""
import os
import http.server
import socketserver
from argparse import ArgumentParser

from cyclus.lib import Env


def make_parser():
    p = ArgumentParser('webapp')
    p.add_argument('-d', '--dir', dest='d',
                   default=os.path.join(Env().install_path, 'share',
                                        'cyclus', 'webapp'),
                   help='directory to host.')
    return p


def run_app(d):
    """Runs the Cyclus web app.

    Parameters
    ----------
    d : str
        Directory to serve.
    """
    os.chdir(d)
    Handler = http.server.SimpleHTTPRequestHandler
    host = 'localhost'
    port = 4200
    httpd = socketserver.TCPServer((host, port), Handler)
    print("serving {} at http://{}:{}".format(d, host, port))
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("Shutting down server.")
    finally:
        httpd.shutdown()


def main(args=None):
    """Main cyclus websapp CLI"""
    p = make_parser()
    ns = p.parse_args()
    run_app(d=ns.d)


if __name__ == '__main__':
    main()