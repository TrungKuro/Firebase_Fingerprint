/* Note: WEB GET VALUE FROM FIREBASE
**
** These functions are automatically executed every time there is a change to the data it is pointing to on Firebase
** Therefore, we only need to call it once at the time of website initialization to get the data from Firebase
*/

/* ------------------------------------------------------------------------- */
/*                                  FIREBASE                                 */
/* ------------------------------------------------------------------------- */

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
var firebaseConfig = {
  apiKey: "AIzaSyCGCNj5Y60_mOvVhGa59Z97Q_By_mz2CMY",
  authDomain: "nodemcu-c6e6d.firebaseapp.com",
  databaseURL: "https://nodemcu-c6e6d-default-rtdb.firebaseio.com",
  projectId: "nodemcu-c6e6d",
  storageBucket: "nodemcu-c6e6d.appspot.com",
  messagingSenderId: "238954531984",
  appId: "1:238954531984:web:86fc0e4646bf4898eedc15",
  measurementId: "G-TB1GL27H5M"
};

/* Initialize Firebase */
firebase.initializeApp(firebaseConfig);

/* Get value from firebase to show it when first run (sync between html and firebase) */
var database = firebase.database();

/* ------------------------------------------------------------------------- */
/*                          Input processing (User)                          */
/* ------------------------------------------------------------------------- */

/* --------------------------------- LOGIN --------------------------------- */
function checkAccess(e) {
  event.preventDefault();

  var username = document.getElementById("uname").value;
  var password = document.getElementById("psw").value;

  if (username === "admin" && password === "123") {
    /* -------------------- Update data, before viewing -------------------- */

    /* WEB GET VALUE FROM FIREBASE, update for "Timesheets" */
    updateTimeSheet();

    /* WEB GET VALUE FROM FIREBASE, update for "Open" */
    database.ref().on("value", function (snap) {
      var getFirebase = snap.val().Open;
      document.getElementById("now-open").innerHTML = "Now Open [" + convertValueToTime(getFirebase) + "]";
    });

    /* WEB GET VALUE FROM FIREBASE, update for "Close" */
    database.ref().on("value", function (snap) {
      var getFirebase = snap.val().Close;
      document.getElementById("now-close").innerHTML = "Now Close [" + convertValueToTime(getFirebase) + "]";
    });

    /* --------------------------------------------------------------------- */
    document.getElementById('access').style.display = "none";
  }
  else {
    alert("This account is not valid!\nPlease log in again.");
  }
}

/* ------------------------------- TIMESHEETS ------------------------------ */

function updateTimeSheet() {
  /* WEB GET VALUE FROM FIREBASE, update for "Total" */
  database.ref("ID").on("value", function (snap) {
    var total = snap.val().Total;

    total = Number(total);

    /* DEBUG */
    // alert("Total: " + total);

    deleteOldTable();

    for (let locate = 1; locate <= total; locate++) {
      /* WEB GET VALUE FROM FIREBASE, update for "Date, ID, Note, Time" */
      database.ref("user/" + locate.toString()).on("value", function (snap) {
        var date = snap.val().Date;
        /* DEBUG */
        // alert("Date: " + date);

        var id = snap.val().ID;
        /* DEBUG */
        // alert("ID: " + id);

        var note = snap.val().Note;
        /* DEBUG */
        // alert("Note: " + note);

        var time = snap.val().Time;
        /* DEBUG */
        // alert("Time: " + time);

        /* WEB GET VALUE FROM FIREBASE, update for "Name" */
        database.ref("ID/" + id).on("value", function (snap) {
          var name = snap.val();
          /* DEBUG */
          // alert("1st: " + locate.toString());
          // alert("Name: " + name);
          // alert("ID: " + id);
          // alert("Date: " + date);
          // alert("Time: " + time);
          // alert("Note: " + note);

          createRowForTable(locate, name, id, date, time, note);
        });
      });
    }
  });
}

/* ---------------------------------- NAME --------------------------------- */

function updateName(e) {
  event.preventDefault();

  var getName = document.getElementById("set-name").value;
  var getID = document.getElementById("set-id").value;

  /* DEBUG */
  // alert(getName + " : " + getID);

  let text = "Do you want to register ID [" + getID + "] with Name '" + getName + "'?";
  if (confirm(text) == true) {
    /* WEB SEND VALUE TO FIREBASE */
    var firebaseRef = firebase.database().ref().child("ID/" + getID);
    firebaseRef.set(getName);

    alert("Sign Up Success!");
  }
}

/* ---------------------------------- OPEN --------------------------------- */

function updateOpen(e) {
  event.preventDefault();

  var getOpen = document.getElementById("set-open").value;

  /* DEBUG */
  // alert(getOpen);
  // alert(convertTimeToValue(getOpen));

  /* WEB SEND VALUE TO FIREBASE */
  var firebaseRef = firebase.database().ref().child("Open");
  firebaseRef.set(convertTimeToValue(getOpen));
}

/* --------------------------------- CLOSE --------------------------------- */

function updateClose(e) {
  event.preventDefault();

  var getClose = document.getElementById("set-close").value;

  /* DEBUG */
  // alert(getClose);
  // alert(convertTimeToValue(getClose));

  /* WEB SEND VALUE TO FIREBASE */
  var firebaseRef = firebase.database().ref().child("Close");
  firebaseRef.set(convertTimeToValue(getClose));
}

/* ------------------------------------------------------------------------- */
/*                                  FUNCTION                                 */
/* ------------------------------------------------------------------------- */

function convertTimeToValue(data) {
  var ok = 3600 * data.substr(0, 2) + 60 * data.substr(3, 2);
  return ok.toString();
}

function convertValueToTime(data) {
  data = Number(data);
  data = data / 60;
  for (let h = 0; h <= 23; h++) {
    let m = data - 60 * h;
    if (m >= 0 && m <= 59) {
      return ((h < 10) ? ("0" + h.toString()) : (h.toString())) + ":" + ((m < 10) ? ("0" + m.toString()) : (m.toString()));
    }
  }
}

/* ------------------------------------------------------------------------- */

function deleteOldTable() {
  /* DEBUG */
  // alert("Delete Old Table");

  var numberRow = document.getElementById("timesheets").rows.length;

  /* DEBUG */
  // alert("Number Row: " + numberRow);

  /* Delete a row, until all table */
  for (let i = numberRow - 1; i >= 1; i--) {
    document.getElementById("timesheets").deleteRow(i);
  }
}

function createRowForTable(locate, name, id, date, time, note) {
  /* DEBUG */
  // alert("Create New Table");

  var row = document.getElementById("timesheets").insertRow(locate);

  /* Insert all (6) cells on a row */
  row.insertCell(0).innerHTML = locate;
  row.insertCell(1).innerHTML = name;
  row.insertCell(2).innerHTML = id;
  row.insertCell(3).innerHTML = date;
  row.insertCell(4).innerHTML = time;
  row.insertCell(5).innerHTML = note;
}
