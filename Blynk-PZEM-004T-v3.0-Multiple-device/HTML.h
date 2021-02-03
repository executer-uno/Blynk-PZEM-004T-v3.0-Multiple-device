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
		//if(confirm("Update config?")){
			setTimeout(function(){ document.location.reload(false); }, 500);
		//}
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

  <div class="buttons">
    <button onclick="location.href='/'" type="button">HOME</button>
  </div>
  <div class="buttons">
	<form action="/reboot" method="post">
		<button name="REBOOT" value="REBOOT">REBOOT</button>
	</form>
  </div>


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
        <td>CURRENT</td>
        <td>POWER</td>
        <td>PF</td>
		<td>ENERGY</td>
      </tr><tr>
        <td id="Tag1">#TAG</td>
        <td id="Cur1">#TAG</td>
        <td id="Pow1">#TAG</td>
        <td id="PFc1">#TAG</td>
        <td id="Eng1">#TAG</td>
    </tr><tr>
        <td id="Tag2">#TAG</td>
        <td id="Cur2">#TAG</td>
        <td id="Pow2">#TAG</td>
        <td id="PFc2">#TAG</td>
        <td id="Eng2">#TAG</td>
      </tr><tr>
        <td id="Tag3">#TAG</td>
        <td id="Cur3">#TAG</td>
        <td id="Pow3">#TAG</td>
        <td id="PFc3">#TAG</td>
        <td id="Eng3">#TAG</td>
      </tr><tr>
        <td id="Tag4">#TAG</td>
        <td id="Cur4">#TAG</td>
        <td id="Pow4">#TAG</td>
        <td id="PFc4">#TAG</td>
        <td id="Eng4">#TAG</td>
      </tr>
    </table>
  </div>

  <div class="buttons">
    <button onclick="location.href='/config'" type="button">CONFIG</button>
  </div>
  </body>

	<script>
	
	//**************************************************************

		fetchdata(1);
		fetchdata(2);
		fetchdata(3);
		fetchdata(4);

		setInterval(function(){
			fetchdata(1);
			fetchdata(2);
			fetchdata(3);
			fetchdata(4);
		}, 10000) ;


		function fetchdata (index) {
	
			var url = "/data";
			var params = "sensor="+ index;// + "&control=2";
			var http = new XMLHttpRequest();
			
			http.open("GET", url+"?"+params, true);
			http.onreadystatechange = function()
			{
				if(http.readyState == 4 && http.status == 200) {
					//alert(http.responseText);
	
					var res = http.responseText.split(";");
	
					document.getElementById("Tag"+index).innerHTML = res[0];
					document.getElementById("Cur"+index).innerHTML = res[1];
					document.getElementById("Pow"+index).innerHTML = res[2];
					document.getElementById("PFc"+index).innerHTML = res[3];
					document.getElementById("Eng"+index).innerHTML = res[4];
	
				}
			}
			http.send(null);
		}



	//****************************************************************

	</script>

</html>
)rawliteral";

#endif /* HTML_H_ */
