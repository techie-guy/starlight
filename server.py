from http.server import SimpleHTTPRequestHandler, HTTPServer
import ssl

class MyHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        # Set headers for cross-origin isolation
        # self.send_header('Cross-Origin-Embedder-Policy', 'credentialless')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        super().end_headers()

# Define the server address and port
HOST = '0.0.0.0'
PORT = 8000

httpd = HTTPServer((HOST, PORT), MyHandler)

httpd.socket = ssl.wrap_socket(httpd.socket,certfile='./0.0.0.0.pem', keyfile='./0.0.0.0-key.pem', server_side=True)

print("Serving HTTPS on port 8000")
httpd.serve_forever()
