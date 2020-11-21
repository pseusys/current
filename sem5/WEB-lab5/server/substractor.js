const fs = require('fs');
const path = require('path');

const key_property = "name";



function get_dataset (file) {
  return JSON.parse(fs.readFileSync(path.join(__dirname, "../storage/" + file + ".json")).toString());
}

function write_dataset (dataset, file) {
  fs.writeFileSync(path.join(__dirname, "../storage/" + file + ".json"), JSON.stringify(dataset));
}


function add_data (data, file) {
  const id = data[key_property];
  const dataset = get_dataset(file);
  if (!dataset[id]) dataset[id] = data;
  else return undefined;
  write_dataset(dataset, file);
  return dataset[id];
}

function get_data (id, file) {
  const dataset = get_dataset(file);
  return dataset[id];
}

function get_datas (file) {
  const dataset = get_dataset(file);
  const datas = [];
  for (const data in dataset) if ((data !== "id") && (data !== "password")) datas.push(dataset[data]);
  return datas;
}

function edit_data (new_data, file) {
  const id = new_data[key_property];
  const data = get_data(id, file);
  if (!data) return data;
  delete_data(id, file);
  return add_data(new_data, file);
}

function delete_data (id, file) {
  const dataset = get_dataset(file);
  delete dataset[id];
  write_dataset(dataset, file);
  return undefined;
}



const brokers_file = "brokers";
const actions_file = "actions";
const passwords_file = "passwords";
const settings_file = "settings";



module.exports.read_settings = function () {
  return get_dataset(settings_file);
}

module.exports.write_settings = function (settings) {
  write_dataset(settings, settings_file);
  return get_dataset(settings_file);
}



module.exports.add_broker = function (username, password) {
  const broker = { name: username, money: 0, isAdmin: false };
  if (add_data({ name: username, password: password }, passwords_file)) return add_data(broker, brokers_file);
  else return undefined;
}

module.exports.check_broker = function (username, password) {
  const pass = get_data(username, passwords_file);
  if (pass.password === password) return get_data(username, brokers_file);
  else return undefined;
}

module.exports.get_broker = function (id) { return get_data(id, brokers_file); }

module.exports.get_brokers = function () { return get_datas(brokers_file); }

module.exports.edit_broker = function (broker) { return edit_data(broker, brokers_file); }

module.exports.delete_broker = function (id) { return delete_data(id, brokers_file); }


module.exports.add_action = function (action) { return add_data(action, actions_file); }

module.exports.get_action = function (id) { return get(id, actions_file); }

module.exports.get_actions = function () { return get_datas(actions_file); }

module.exports.edit_action = function (action) { return edit_data(action, actions_file); }

module.exports.delete_action = function (id) { return delete_data(id, actions_file); }
