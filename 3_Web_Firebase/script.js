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

function checkAccess(e) {
  event.preventDefault();

  var username = document.getElementById("uname").value;
  var password = document.getElementById("psw").value;

  if (username === "admin" && password === "123") {
    document.getElementById('access').style.display = "none";
  }
  else {
    alert("This account is not valid!\nPlease log in again.");
  }
}

function updateName(e) {
  event.preventDefault();

  var getName = document.getElementById("set-name").value;
  var getID = document.getElementById("set-id").value;

  // alert(getName + " : " + getID);

  /* WEB SEND VALUE TO FIREBASE */
  var firebaseRef = firebase.database().ref().child("ID/" + getID);
  firebaseRef.set(getName);
}

function updateOpen(e) {
  event.preventDefault();

  var getOpen = document.getElementById("set-open").value;

  // alert(getOpen);
  // alert(convertTime(getOpen));

  /* WEB SEND VALUE TO FIREBASE */
  var firebaseRef = firebase.database().ref().child("Open");
  firebaseRef.set(convertTime(getOpen));
}

function updateClose(e) {
  event.preventDefault();

  var getClose = document.getElementById("set-close").value;

  // alert(getClose);
  // alert(convertTime(getClose));

  /* WEB SEND VALUE TO FIREBASE */
  var firebaseRef = firebase.database().ref().child("Close");
  firebaseRef.set(convertTime(getClose));
}

/* ------------------------------------------------------------------------- */
/*                                  FUNCTION                                 */
/* ------------------------------------------------------------------------- */

function convertTime(data) {
  var ok = 3600 * data.substr(0, 2) + 60 * data.substr(3, 2);
  return ok.toString();
}
