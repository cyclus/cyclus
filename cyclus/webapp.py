"""Launches a basic webapp for cyclus simulations."""
import os
import http.server
import socketserver

from cyclus.lib import Env


def run_app():
    """Runs the Cyclus web app."""
    d = os.path.join(Env().install_path, 'share', 'cyclus', 'webapp')
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
    run_app()


if __name__ == '__main__':
    main()