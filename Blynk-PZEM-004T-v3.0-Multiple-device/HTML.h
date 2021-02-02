/*
 * HTML.h
 *
 *  Created on: Jan 12, 2021
 *      Author: E_CAD
 */

#ifndef HTML_H_
#define HTML_H_



// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
const char config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saved value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500); 
    }
  </script></head><body>

	  <table>
	  <tr>
		<td>
		  <h2>Device Tag</h2>
		</td>
		<td>
		  <h2>Modbus Adress</h2>
		</td>
		<td>
		  <h2>Measure Gain</h2>
		</td>
	  </tr>
	  <tr>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevTag1" value="%DevTag1%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevAdr1" value="%DevAdr1%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevGain1" value="%DevGain1%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
	  </tr>
	  <tr>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevTag2" value="%DevTag2%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevAdr2" value="%DevAdr2%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevGain2" value="%DevGain2%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
	  </tr>
	  <tr>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevTag3" value="%DevTag3%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevAdr3" value="%DevAdr3%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevGain3" value="%DevGain3%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
	  </tr>
	  <tr>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevTag4" value="%DevTag4%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevAdr4" value="%DevAdr4%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
		<td>
		  <form action="/update" target="hidden-form">
			<input type="text" name="DevGain4" value="%DevGain4%">
			<input type="submit" value=">" onclick="submitMessage()">
		  </form>
		</td>
	  </tr>  
	  </table>
	  
	<br>
	<form action="/update" target="hidden-form">
	  <input type="text" name="MaxSendPeriod" value="%MaxSendPeriod%">
	  <input type="submit" value=">" onclick="submitMessage()">
	</form>




  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>
)rawliteral";

// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>PZEM Measurements</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  
  <div class="measurements">
    <table>
       <tr>
        <td>TAG</td>
        <td>POWER</td>
        <td>CURRENT</td>
      </tr><tr>
        <td>#TAG</td>
        <td>#POWER</td>
        <td>#CURRENT</td>
      </tr><tr>
        <td>#TAG</td>
        <td>#POWER</td>
        <td>#CURRENT</td>
      </tr><tr>
        <td>#TAG</td>
        <td>#POWER</td>
        <td>#CURRENT</td>
      </tr>
    </table>
  </div>

  <div class="buttons">
    <button onclick="location.href='/config'" type="button">CONFIG</button>
  </div>
  </body></html>
)rawliteral";

#endif /* HTML_H_ */
