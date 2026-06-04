from fastapi import FastAPI
from fastapi.responses import HTMLResponse
from tinydb import TinyDB
from datetime import datetime
import random

app = FastAPI()

db = TinyDB("db.json")

html = """
<!DOCTYPE html>
<html>
<head>
<title>Earthquake Monitoring Wave App</title>

<style>

*{
    margin:0;
    padding:0;
    box-sizing:border-box;
}

body{
    font-family:Arial,sans-serif;
    background:linear-gradient(135deg,#0f2027,#203a43,#2c5364);
    color:white;
    min-height:100vh;
}

.container{
    width:95%;
    margin:auto;
    padding:20px;
}

h1{
    text-align:center;
    margin-bottom:20px;
}

.dashboard{
    display:grid;
    grid-template-columns:repeat(auto-fit,minmax(250px,1fr));
    gap:20px;
}

.card{
    background:white;
    color:black;
    padding:20px;
    border-radius:15px;
    text-align:center;
}

.wave{
    width:100%;
    height:120px;
    margin-top:20px;
    border-radius:15px;
    background:linear-gradient(90deg,#00c6ff,#0072ff);
    overflow:hidden;
    position:relative;
}

.wave::before{
    content:'';
    position:absolute;
    width:200%;
    height:200%;
    background:rgba(255,255,255,0.2);
    border-radius:40%;
    animation:wave 8s linear infinite;
}

@keyframes wave{
    from{
        transform:translateX(-50%) rotate(0deg);
    }
    to{
        transform:translateX(0%) rotate(360deg);
    }
}

.form-box{
    background:white;
    color:black;
    margin-top:20px;
    padding:20px;
    border-radius:15px;
}

input{
    padding:10px;
    margin:5px;
    width:220px;
}

button{
    padding:10px 15px;
    border:none;
    border-radius:6px;
    cursor:pointer;
    color:white;
}

.add-btn{
    background:green;
}

.edit-btn{
    background:#2196F3;
}

.delete-btn{
    background:red;
}

.sim-btn{
    background:#ff5722;
}

.table-box{
    background:white;
    color:black;
    margin-top:20px;
    padding:20px;
    border-radius:15px;
    overflow-x:auto;
}

table{
    width:100%;
    border-collapse:collapse;
}

th{
    background:#ff5722;
    color:white;
}

th,td{
    border:1px solid #ddd;
    padding:10px;
    text-align:center;
}

.high{
    color:red;
    font-weight:bold;
}

.medium{
    color:orange;
    font-weight:bold;
}

.low{
    color:green;
    font-weight:bold;
}

</style>
</head>

<body>

<div class="container">

<h1>🌍 Earthquake Monitoring Wave App</h1>

<div class="dashboard">

<div class="card">
<h2>Total Records</h2>
<h1 id="total">0</h1>
</div>

<div class="card">
<h2>Highest Magnitude</h2>
<h1 id="highest">0</h1>
</div>

<div class="card">
<h2>Latest Location</h2>
<h1 id="latest">-</h1>
</div>

</div>

<div class="wave"></div>

<div class="form-box">

<h2>Add Earthquake Record</h2>
<br>

<input type="number" id="magnitude" placeholder="Magnitude" step="0.1">

<input type="text" id="location" placeholder="Location">

<button class="add-btn" onclick="addRecord()">
Add Record
</button>

<button class="sim-btn" onclick="simulateData()">
Generate Random Data
</button>

</div>

<div class="table-box">

<h2>Live Earthquake Records</h2>

<br>

<table>

<thead>
<tr>
<th>ID</th>
<th>Magnitude</th>
<th>Location</th>
<th>Risk</th>
<th>Timestamp</th>
<th>Actions</th>
</tr>
</thead>

<tbody id="tableData"></tbody>

</table>

</div>

</div>

<script>


    return data
    async function loadData(){

    let response = await fetch('/earthquakes');
    let data = await response.json();

    let rows = "";
    let highest = 0;
    let latest = "-";

    data.reverse().forEach(item=>{

        if(item.magnitude > highest)
            highest = item.magnitude;

        latest = item.location;

        let risk = "Low";
        let cls = "low";

        if(item.magnitude >= 6){
            risk = "High";
            cls = "high";
        }
        else if(item.magnitude >= 4){
            risk = "Medium";
            cls = "medium";
        }

        rows += `
        <tr>
            <td>${item.id}</td>
            <td>${item.magnitude}</td>
            <td>${item.location}</td>
            <td class="${cls}">${risk}</td>
            <td>${item.timestamp}</td>

            <td>

            <button class="edit-btn"
            onclick="editRecord(${item.id},
            '${item.location}',
            ${item.magnitude})">
            Edit
            </button>

            <button class="delete-btn"
            onclick="deleteRecord(${item.id})">
            Delete
            </button>

            </td>

        </tr>
        `;
    });

    document.getElementById("tableData").innerHTML = rows;
    document.getElementById("total").innerText = data.length;
    document.getElementById("highest").innerText = highest;
    document.getElementById("latest").innerText = latest;
}

async function addRecord(){

    let magnitude =
    document.getElementById("magnitude").value;

    let location =
    document.getElementById("location").value;

    await fetch(
    `/earthquake?magnitude=${magnitude}&location=${location}`,
    {
        method:"POST"
    });

    loadData();
}

async function editRecord(id, location, magnitude){

    let newMagnitude =
    prompt("Enter New Magnitude", magnitude);

    let newLocation =
    prompt("Enter New Location", location);

    if(newMagnitude==null || newLocation==null)
        return;

    await fetch(
    `/earthquake/${id}?magnitude=${newMagnitude}&location=${newLocation}`,
    {
        method:"PUT"
    });

    loadData();
}

async function deleteRecord(id){

    if(!confirm("Delete Record?"))
        return;

    await fetch(
    `/earthquake/${id}`,
    {
        method:"DELETE"
    });

    loadData();
}

async function simulateData(){

    await fetch('/simulate');

    loadData();
}

loadData();

setInterval(loadData,3000);

</script>

</body>
</html>
"""

@app.get("/", response_class=HTMLResponse)
async def home():
    return html


@app.get("/earthquakes")
async def get_earthquakes():

    records = []

    for item in db:
        row = item.copy()
        row["id"] = item.doc_id
        records.append(row)

    return records




