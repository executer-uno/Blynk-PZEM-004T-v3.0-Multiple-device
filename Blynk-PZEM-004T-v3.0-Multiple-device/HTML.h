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
<!DOCTYPE HTML>
<html>

<head>
    <title>›ÎÂÍÚËÍ‡. Õ‡ÒÚÓÈÍË</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Montserrat&display=swap" rel="stylesheet">
    
	<style>
		@import url("https://netdna.bootstrapcdn.com/font-awesome/4.0.1/css/font-awesome.css");
		
		* {
		  margin: 0;
		  padding: 0;
		
		  font-family: "Montserrat", sans-serif;
		  font-style: normal;
		  font-weight: normal;
		
		  color: #bbb;
		}
		
		body {
		  height: 100%%;
		  width: 100%%;
		  min-height: 100%%;
		  font-size: 12px;
		  line-height: 16px;
		  background: #111;
		}
		
		h1 {
		  font-size: 20px;
		  line-height: 24px;
		}
		
		input[type="text"] {
		  margin-left: 10px;
		  border: 0px solid;
		  background: transparent;
			
		  width: 100%%;
		}
		
		table {
		  border-spacing: 0;
		  width: 100%%;
		  text-align: center;
		
		  font-size: 12px;
		  line-height: 16px;
		}
		th,
		td {
		  padding: 10px 20px;
		  border-right: 1px solid #909090;
		  border-bottom: 1px solid #909090;
		}
		
		button {
		  margin-left: 15px;
		  padding: 10px;
		  background: #f2f2f2;
		  color: #141313;
		  min-width: 100px;
		}
		
		.title {
		  height: 85px;
		  padding: 15px 15px 0 15px;
		  text-align: right;
		}
		
		.measurements {
		  padding: 0 15px;
		}
		
		.buttons {
		  padding: 15px;
		  position: absolute;
		  bottom: 0px;
		  right: 0px;
		}
		
		th {
		  font-weight: bold;
		}
		
		tr:nth-child(even) {
		  background: #222;
		}
		
		td.spacer {
		  background: black;
		  border-right: 1px solid black;
		}
		
		td:last-child,
		th:last-child {
		  border-right: 0px solid;
		}
		
		td.right-span {
		  border-right: 0px solid;
		}
		
		.data-row:hover {
		  background: #444;
		}
		
		.icon {
		  font: 20px fontawesome;
		  color: #141313;
		  padding-right: 10px;
		}
		
		.attention {
		  background: #eb5757;
		}

	</style>	
</head>

<body>
    <div class="title">
        <h1>Õ¿—“–Œ… »</h1>
    </div>
    <div class="measurements">

        <table class="zebra">
            <tr>
                <th>DEVICE TAG</th>
                <th>MODBUS ADRESS</th>
                <th>MEASURE GAIN</th>
            </tr>
            <tr>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevTag1" value="%DevTag1%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevAdr1" value="%DevAdr1%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevGain1" value="%DevGain1%">
                    </form>
                </td>
            </tr>
            <tr>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevTag2" value="%DevTag2%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevAdr2" value="%DevAdr2%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevGain2" value="%DevGain2%">
                    </form>
                </td>
            </tr>
            <tr>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevTag3" value="%DevTag3%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevAdr3" value="%DevAdr3%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevGain3" value="%DevGain3%">
                    </form>
                </td>
            </tr>
            <tr>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevTag4" value="%DevTag4%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevAdr4" value="%DevAdr4%">
                    </form>
                </td>
                <td>
                    <form action="/update" target="hidden-form">
                        <input type="text" name="DevGain4" value="%DevGain4%">
                    </form>
                </td>
            </tr>
            <tr>
                <td>
                    MAX PERIOD
                </td>
                <td class="right-span">
                    <form action="/update" target="hidden-form">
                        <input type="text" name="MaxSendPeriod" value="%MaxSendPeriod%">
                    </form>
                </td>
                <td>
                </td>
            </tr>
        </table>



    </div>
    
    
    <div class="buttons">

        <button onclick="location.href='/reboot'" type="button" class="attention"><span class="icon">&#xf021</span>–≈—“¿–“</button>
        <button onclick="location.href='/'" type="button"><span class="icon">&#xf015</span>ƒŒÃŒ…</button>

    </div>

    <!-- <iframe style="display:none" name="hidden-form"></iframe> -->
</body>

</html>
)rawliteral";

// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>›ÎÂÍÚËÍ‡. »ÁÏÂÂÌËˇ</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Montserrat&display=swap" rel="stylesheet">

	<STYLE>
		@import url("https://netdna.bootstrapcdn.com/font-awesome/4.0.1/css/font-awesome.css");
		
		* {
		  margin: 0;
		  padding: 0;
		
		  font-family: "Montserrat", sans-serif;
		  font-style: normal;
		  font-weight: normal;
		
		  color: #bbb;
		}
		
		body {
		  height: 100%%;
		  width: 100%%;
		  min-height: 100%%;
		  font-size: 12px;
		  line-height: 16px;
		  background: #111;
		}
		
		h1 {
		  font-size: 20px;
		  line-height: 24px;
		}
		
		table {
		  border-spacing: 0;
		  width: 100%%;
		  text-align: center;
		}
		th,
		td {
		  padding: 10px 20px;
		  border-right: 1px solid #909090;
		  border-bottom: 1px solid #909090;
		}
		
		button {
		  padding: 10px;
		  background: #f2f2f2;
		  color: #141313;
		
		  min-width: 100px;
		}
		
		.title {
		  height: 85px;
		  padding: 15px 15px 0 15px;
		  text-align: right;
		}
		
		.measurements {
		  padding: 0 15px;
		}
		
		.buttons {
		  padding: 15px;
		  position: absolute;
		  bottom: 0px;
		  right: 0px;
		}
		
		th {
		  font-weight: bold;
		}
		th:first-child {
		  text-align: right;
		}
		
		tr:nth-child(even) {
		  background: #222;
		}
		
		td.spacer {
		  background: black;
		  border-right: 1px solid black;
		}
		
		td:last-child,
		th:last-child {
		  border-right: 0px solid;
		}
		
		td.right-span {
		  border-right: 0px solid;
		}
		
		.data-row:hover {
		  background: #444;
		}
		
		.icon {
		  font: 20px fontawesome;
		  color: #141313;
		  padding-right: 10px;
		}

	</STYLE>
</head>

<body>
    <div class="title">
        <h1>»«Ã≈–≈Õ»ﬂ</h1>
    </div>
    <div class="measurements">
        <table class="zebra">
            <tr>
                <th></th>
                <th>CURRENT</th>
                <th>POWER</th>
                <th>PF</th>
                <th>ENERGY</th>
            </tr>
            <tr class="data-row">
                <th id="Tag1">BATH</th>
                <td id="Cur1">#TAG</td>
                <td id="Pow1">#TAG</td>
                <td id="PFc1">#TAG</td>
                <td id="Eng1">#TAG</td>
            </tr>
            <tr class="data-row">
                <th id="Tag2">INLET</th>
                <td id="Cur2">#TAG</td>
                <td id="Pow2">#TAG</td>
                <td id="PFc2">#TAG</td>
                <td id="Eng2">#TAG</td>
            </tr>
            <tr class="data-row">
                <th id="Tag3">LIGHT</th>
                <td id="Cur3">#TAG</td>
                <td id="Pow3">#TAG</td>
                <td id="PFc3">#TAG</td>
                <td id="Eng3">#TAG</td>
            </tr>
            <tr class="data-row">
                <th id="Tag4">FRIDGE</th>
                <td id="Cur4">#TAG</td>
                <td id="Pow4">#TAG</td>
                <td id="PFc4">#TAG</td>
                <td id="Eng4">#TAG</td>
            </tr>
            <tr>
                <td class="spacer" colspan="5"><br></td>
            </tr>
            <tr class="data-row">
                <th id="Tag4">VOLTAGE</th>
                <td id="Cur4" class="right-span">#TAG</td>
                <td colspan="3"></td>
            </tr>
            <tr class="data-row">
                <th id="Tag4">FREQ</th>
                <td id="Cur4" class="right-span">#TAG</td>
                <td colspan="3"></td>
            </tr>
        </table>
    </div>


    <div class="buttons">
        <button onclick="location.href='/config'" type="button"><span class="icon">&#xf013</span>Õ¿—“–Œ… ¿</button>
    </div>

</body>

<script>
    //**************************************************************

    fetchdata(1);
    fetchdata(2);
    fetchdata(3);
    fetchdata(4);

    setInterval(function () {
        fetchdata(1);
        fetchdata(2);
        fetchdata(3);
        fetchdata(4);
    }, 10000);


    function fetchdata(index) {

        var url = "/data";
        var params = "sensor=" + index; // + "&control=2";
        var http = new XMLHttpRequest();

        http.open("GET", url + "?" + params, true);
        http.onreadystatechange = function () {
            if (http.readyState == 4 && http.status == 200) {
                //alert(http.responseText);

                var res = http.responseText.split(";");

                document.getElementById("Tag" + index).innerHTML = res[0];
                document.getElementById("Cur" + index).innerHTML = res[1];
                document.getElementById("Pow" + index).innerHTML = res[2];
                document.getElementById("PFc" + index).innerHTML = res[3];
                document.getElementById("Eng" + index).innerHTML = res[4];

            }
        }
        http.send(null);
    }



    //****************************************************************

</script>

</html>
)rawliteral";

#endif /* HTML_H_ */
