import Physics
import sys
import os
import cgi
from math import sqrt
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qsl


class MyHandler(BaseHTTPRequestHandler):
   def do_GET(self):
       parsed = urlparse(self.path)

       # parse URL
       parsed  = urlparse( self.path );

       # check if webpages matches
       if parsed.path in [ '/shoot.html' ]:

           # retreive the HTML file
           fp = open( '.'+self.path );
           content = fp.read();

           # generate headers
           self.send_response( 200 ); # OK
           self.send_header( "Content-type", "text/html" );
           self.send_header( "Content-length", len( content ) );
           self.end_headers();

           # send to browser
           self.wfile.write( bytes( content, "utf-8" ) );
           fp.close();

       elif parsed.path.startswith('/table-') and parsed.path.endswith('.svg'):
           # retrieve the table-?.svg files
           fp = open( '.'+self.path, 'rb' );
          
           content = fp.read()
           self.send_response(200)
           self.send_header("Content-type", "image/svg+xml")
           self.send_header("Content-length", len(content))
           self.end_headers()
           self.wfile.write(content)

       else:
           # generate 404 for other requests
           self.send_error(404, "File not found")


   def do_POST(self):
       # parse url
       parsed = urlparse(self.path)

       if parsed.path in ['/display.html']:
           # get data as mime format
           form = cgi.FieldStorage(fp=self.rfile,
                                   headers=self.headers,
                                   environ={'REQUEST_METHOD': 'POST',
                                           'CONTENT_TYPE':
                                           self.headers['Content-Type'],
                                           })

           # delete all table-?.svg files
           for filename in os.listdir('.'):
               if filename.startswith('table-') and filename.endswith('.svg'):
                   os.remove(filename)

           # generate the svg files from the data
           self.generateSvg(form)

           # generate html response
           html_response = self.htmlResponse(form)

           # generate headers
           self.send_response(200) 
           self.send_header("Content-type", "text/html")
           self.send_header("Content-length", len(html_response))
           self.end_headers()
           self.wfile.write(bytes(html_response, "utf-8"))

   def calcAcc(self, form_data):
       # calculate acceleration of rb based on form data
       rb_acc_x, rb_acc_y = 0.0, 0.0

       rb_dx = float(form_data['rb_dx'].value)
       rb_dy = float(form_data['rb_dy'].value)

       speed = sqrt(rb_dx ** 2 + rb_dy ** 2)

       if speed > 0.01:
           rb_acc_x = -(rb_dx / speed) * 150.0
           rb_acc_y = -(rb_dy / speed) * 150.0

       return rb_acc_x, rb_acc_y

   def generateSvg(self, form_data):
        # create the still ball
        sb_x = float(form_data['sb_x'].value)
        sb_y = float(form_data['sb_y'].value)
        sb_pos = Physics.Coordinate(sb_x, sb_y)
        sb_num = int(form_data['sb_number'].value)
        sb = Physics.StillBall(sb_num, sb_pos)

        # create the rolling ball
        rb_x = float(form_data['rb_x'].value)
        rb_y = float(form_data['rb_y'].value)
        rb_pos = Physics.Coordinate(rb_x, rb_y)
        rb_acc_x, rb_acc_y = self.calcAcc(form_data)
        rb_vel = Physics.Coordinate(float(form_data['rb_dx'].value), float(form_data['rb_dy'].value))
        rb_acc = Physics.Coordinate(rb_acc_x, rb_acc_y)
        rb_num = int(form_data['rb_number'].value)
        rb = Physics.RollingBall(rb_num, rb_pos, rb_vel, rb_acc)

        # add both balls to the table
        table = Physics.Table()
        table += sb
        table += rb

        # save the svg files to the server dir
        index = 0
        while table:
            filename = f"table-{index}.svg"
            with open(filename, "w") as f:
                f.write(table.svg())
            table = table.segment()   
            index += 1

   def htmlResponse(self, form_data):
        # construct html page with simulation info 
        htmlContent = "<html><head></head><body>"
        htmlContent += "<h1>Billiards Simulation</h1>"
        htmlContent += "<h2>Original Positions & Velocity</h2>"
        htmlContent += f"<p>Still ball position: ({form_data['sb_x'].value}, {form_data['sb_y'].value})</p>"
        htmlContent += f"<p>Rolling ball position: ({form_data['rb_x'].value}, {form_data['rb_y'].value})</p>"
        htmlContent += f"<p>Rolling ball velocity: ({form_data['rb_dx'].value}, {form_data['rb_dy'].value})</p>"
        htmlContent += f"<p>Rolling ball acceleration: {self.calcAcc(form_data)}</p>"
        
        # count how many svg files were made
        numFiles = sum(1 for filename in os.listdir('.') if filename.startswith('table-') and filename.endswith('.svg'))

        # add svg images in order
        htmlContent += "<h2>Simulation segments</h2>"
        htmlContent += "<div style='display: flex;'>"
        for i in range(numFiles):
            filename = f"table-{i}.svg"
            if os.path.exists(filename):
                htmlContent += f"<img src='{filename}' alt='Table SVG' width='400' style='margin-right: 10px;'>"
        htmlContent += "</div>"

        # add link to go back
        htmlContent += "<br>"
        htmlContent += "<a href='/shoot.html'>Back</a>"
        htmlContent += "</body></html>"

        return htmlContent


if __name__ == "__main__":
   httpd = HTTPServer(('localhost', int(sys.argv[1])), MyHandler)
   print("Server listening on port:", int(sys.argv[1]))
   httpd.serve_forever()
