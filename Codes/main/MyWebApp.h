#ifndef MyWebApp
#define MyWebApp

const char head_CSSstyle[] = R"---(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Kettle</title>

    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: Arial, Helvetica, sans-serif;
        }

        body {
            background-color: #f9e9ff;
            margin: 20px;
        }

        .header-layout {
            text-align: center;
            margin-bottom: 20px;
        }

        .header-layout h1 {
            color: #007bff;
        }

        .form-layout {
            max-width: 600px;
            margin: 0 auto;
            background-color: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            align-items: center;
            display: flex;
            justify-content: center;
        }

        .get-time input[type="date"] {
            padding: 10px;
            border: 2px solid #ccc;
            border-radius: 5px;
            width: 100%;
            /* Adjust width as needed */
            max-width: 100%;
            transition: border-color 0.3s ease;
        }

        .get-time input[type="date"]:focus {
            border-color: #007bff;
            outline: none;
        }

        .get-time-form h2 {
            margin-bottom: 20px;
            text-align: center;
            color: #333;
        }

        .get-time-label {
            margin-bottom: 10px;
        }

        .get-time-label h3 {
            font-size: 18px;
            margin-bottom: 5px;
            color: #555;
        }

        .get-time {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
        }

        .get-time h5 {
            margin-right: 10px;
            color: #333;
        }

        .get-time input[type="number"],
        .get-time select {
            padding: 10px;
            border: 2px solid #ccc;
            border-radius: 5px;
            width: 80px;
            max-width: 100%;
            transition: border-color 0.3s ease;
            margin-right: 10px;
        }

        .get-time select {
            width: 65px;
        }

        .get-time input[type="number"]:focus,
        .get-time select:focus {
            border-color: #007bff;
            outline: none;
            font-size: 18px;
        }

        .submit-btn {
            text-align: center;
        }

        .submit-btn input[type="submit"] {
            padding: 10px 20px;
            font-size: 15px;
            font-weight: bold;
            color: #fff;
            background-color: #007bff;
            border: none;
            border-radius: 30px;
            cursor: pointer;
            text-decoration: none;
            transition: background-color 0.3s ease;
        }

        .submit-btn input[type="submit"]:hover {
            background-color: #0056b3;
        }

        .error {
            border: 2px solid #ff3333 !important;
        }

        .active-container {
            max-width: 600px;
            /* Adjust the max-width to your preference */
            margin: 20px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .active-container h1 {
            text-align: center;
            margin-bottom: 20px;
        }

        .active-container table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 10px;
        }

        .active-container th,
        .active-container td {
            padding: 12px;
            border-bottom: 1px solid #ddd;
            text-align: center;
        }

        .active-container th {
            background-color: #f2f2f2;
            font-weight: bold;
        }

        .edit-btn {
            text-align: center;
            margin-top: 20px;
        }

        .edit-btn button {
            padding: 10px 20px;
            background-color: #007bff;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }

        .edit-btn button:hover {
            background-color: #0056b3;
        }

        .cancel-btn {
            background-color: #dc3545;
            color: #fff;
            border: none;
            border-radius: 5px;
            padding: 5px 10px;
            cursor: pointer;
        }

        .cancel-btn:hover {
            background-color: #c82333;
        }
    </style>
</head>
)---";

const char body_form_layout_A[] = R"---(
<body>
    <div class="header-layout">
        <h1>Smart Electric Kettle</h1>
        <div id="current-time"></div>
    </div>

    <div class="form-layout">
        <div class="get-time-form">
            <form method='post' action='/submit' onsubmit="validateForm(event)">
                <div class="label-setTimeNow">
                    <h2>Set time now</h2>
                </div>

                <div class="get-time-label" id="label-date">
                    <label for="date">
                        <h3>Date:</h3>
                    </label>
                </div>

                <div class="get-time" id="date-layout">
                    <input type="date" id="date" name="date" required>
                </div>

                <div class="get-time-label" id="label-startTime">
                    <label for="endTime">
                        <h3>Start time:</h3>
                    </label>
                </div>

                <div class="get-time" id="start-time-layout">
                    <h5>Hour:</h5> <input placeholder="Start Hour" id="startHour" type="number" name='startHour' min="1"
                        max="12" required>
                    <h5>Minute:</h5> <input placeholder="Start Minute" id="startMinute" type="number" name='startMinute'
                        min="0" max="59" required>

                    <select name="sMeridiem" required>
                        <option value="AM">AM</option>
                        <option value="PM">PM</option>
                    </select>
                </div>

                <div class="get-time-label" id="label-endTime">
                    <label for="endTime">
                        <h3>End time:</h3>
                    </label>
                </div>

                <div class="get-time" id="end-time-layout">
                    <h5>Hour:</h5> <input placeholder="End Hour" id="endHour" type="number" name='endHour' min="1"
                        max="12" required>
                    <h5>Minute:</h5> <input placeholder="End Minute" id="endMinute" type="number" name='endMinute'
                        min="0" max="59" required>

                    <select name="eMeridiem" required>
                        <option value="AM">AM</option>
                        <option value="PM">PM</option>
                    </select>
                </div>

                <div class="submit-btn" id="submit-btn">
                    <input type='submit' name="formSubmit" value='Submit' onclick="confirmSubmission()">
                </div>
            </form>
        </div>
    </div>
)---";

const char body_activeContainer_B[] = R"---(
  <div class="active-container">
    <h2>Active Alarm</h2>
    <table>
      <thead>
        <tr>
          <th>Date</th>
          <th>Start Time</th>
          <th>End Time</th>
          <th>Action</th>
        </tr>
      </thead>
)---";

const char body_activeContainer_C[] = R"---(
      <tbody>
        <tr>
)---";

const char body_activeContainer_D_a[] = R"---(
        <td>
          <form method='post' action='/request_cancel'>
            <input class="cancel-btn" type='submit' name="formSubmit" value='cancel'>
          </form>
        </td>
)---";

const char body_activeContainer_D_b[] = R"---(
        <td>
          <form method='post' action='/request_cancel'>
            <input class="cancel-btn" type='submit' name="formSubmit" value='cancel' disabled>
          </form>
        </td>
)---";

const char body_activeContainer_E[] = R"---(   
        </tr>
      </tbody>          
    </table>
  
)---";

const char body_activeContainer_F[] = R"---(
  </div>
)---";

const char body_script_F[] = R"---(
    <script>
        function validateForm(event) {
            var sHourInput = document.getElementById("startHour");
            var sMinInput = document.getElementById("startMinute");
            var sMerid = document.getElementById("sMeridiem");
            var eHourInput = document.getElementById("endHour");
            var eMinInput = document.getElementById("endMinute");
            var eMerid = document.getElementById("eMeridiem");

            // Convert the selected time to minutes for comparison
            var selectedStartTime = parseInt(sHourInput.value) * 60 + parseInt(sMinInput.value);
            var selectedEndTime = parseInt(eHourInput.value) * 60 + parseInt(eMinInput.value);

            var timeDifference = selectedEndTime - selectedStartTime;

            if (timeDifference === 0) {
                alert("Start time and end time cannot be the same. Please adjust.");
                event.preventDefault(); // Prevent form submission
            }
        }

        function confirmSubmission() {
            if (confirm("Are you sure you want to submit the form?")) {
                document.querySelector(".get-time-form").submit();
            } else {
                event.preventDefault();
                return false;
            }
        }

        function updateCurrentTime() {
            var currentTime = new Date(); // Get the current date and time
            var options = { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric', hour: '2-digit', minute: '2-digit', second: '2-digit', hour12: true };
            var formattedTime = currentTime.toLocaleTimeString('en-US', options); // Format the time as per your preference

            document.getElementById("current-time").textContent = "Current Local Time: " + formattedTime; // Update the element with id "current-time"
        }

        // Call the updateCurrentTime function initially and then update every second (1000 milliseconds)
        updateCurrentTime(); // Call initially
        setInterval(updateCurrentTime, 1000); 
    </script>
</body>

</html>
)---";


const char successMessage[] = R"---(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Success</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f9f9f9;
            text-align: center;
            padding-top: 50px;
        }
        .success-container {
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            padding: 30px;
            max-width: 400px;
            margin: 0 auto;
        }
        h1 {
            color: #007bff;
            margin-bottom: 20px;
        }
        a {
            color: #007bff;
            text-decoration: none;
            transition: color 0.3s ease;
        }
        a:hover {
            color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="success-container">
        <h1>Submission has been successful!</h1>
        <a href="http://192.168.1.25/" onclick="history.back()">Go back</a>
    </div>
</body>
</html>
)---";

const char errorMessageA[] = R"---(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Error</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f9f9f9;
            text-align: center;
            padding-top: 50px;
        }
        .error-container {
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            padding: 30px;
            max-width: 400px;
            margin: 0 auto;
        }
        h1 {
            color: #333;
            margin-bottom: 20px;
        }
        h3 {
            color: #ff3333;
            margin-bottom: 30px;
        }
        a {
            color: #007bff;
            text-decoration: none;
            transition: color 0.3s ease;
        }
        a:hover {
            color: #0056b3;
        }
    </style>
</head>

<body>
    <div class="error-container">
        <h1>Error</h1>
)---";

const char errorMessageC[] = R"---(
      <a href="http://192.168.1.25/" onclick="history.back()">Go back</a>
    </div>
</body>
</html>
)---";

const char cancellationSuccessMessage[] = R"---(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Success</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f9f9f9;
            text-align: center;
            padding-top: 50px;
        }
        .success-container {
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            padding: 30px;
            max-width: 400px;
            margin: 0 auto;
        }
        h1 {
            color: #007bff;
            margin-bottom: 20px;
        }
        a {
            color: #007bff;
            text-decoration: none;
            transition: color 0.3s ease;
        }
        a:hover {
            color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="success-container">
        <h1>Has been successfully cancelled!</h1>
        <a href="http://192.168.1.25/" onclick="history.back()">Go back</a>
    </div>
</body>
</html>
)---";
#endif
