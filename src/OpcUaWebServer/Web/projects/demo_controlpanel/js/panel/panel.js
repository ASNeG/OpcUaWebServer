/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Samuel Huebl (samuel.huebl@asneg.de)
*/

// ##############################################
//  List Panel
// ##############################################

function ListPanel(callbackClickEvent) {

	// **********************************************
	//  public

	this.addVariable = function(html_ul_id, valueName, displayName) {
		var span = document.createElement("span");
		span.setAttribute("id", "variable_li_span_" + valueName);
		span.appendChild(document.createTextNode("Wird nicht angezeigt."));
		span.style.color = "#ff0000";
		
		var em = document.createElement("em");
		em.appendChild(document.createTextNode(valueName));

		var a = document.createElement("a");
		a.setAttribute("href", "#");
		a.onclick = function () { callbackClickEvent(valueName); return false; }
		a.appendChild(document.createTextNode(displayName));
		a.appendChild(em);
		a.appendChild(span);

		var li = document.createElement("li");
		li.setAttribute("class","w3-row");
		li.appendChild(a);

		var ul = document.getElementById(html_ul_id);
		ul.appendChild(li);
	}

	this.clearList = function(html_ul_id) {
		var ul = document.getElementById(html_ul_id);
		ul.innerHTML = "";
	}

	this.changeStatus = function(valueName, counter) {
		var span = document.getElementById("variable_li_span_" + valueName);
		
		if (span == null) {
			return;
		}

		if (counter > 0) {
			span.style.color = "#008000";
			span.innerHTML = "Wird " + counter + "x angezeigt."
		} else {
			span.style.color = "#ff0000";
			span.innerHTML = "Wird nicht angezeigt."
		}
	}
}

// ##############################################
//  Table Panel
// ##############################################

function TablePanel(callbackWriteEvent) {

	// **********************************************
	//  public

	this.createTable = function(cardId) {
		var close = document.createElement("th");
		var name = document.createElement("th");
		name.appendChild(document.createTextNode("DisplayName"));
		var value = document.createElement("th");
		value.appendChild(document.createTextNode("Value"));
		var edit = document.createElement("th");
		var empty = document.createElement("th");

		var tr = document.createElement("tr");
		tr.appendChild(close);
		tr.appendChild(name);
		tr.appendChild(value);
		tr.appendChild(edit);
		tr.appendChild(empty);

		var thead = document.createElement("thead");
		thead.appendChild(tr);

		var tbody = document.createElement("tbody");
		tbody.setAttribute("id", "table_cardid_" + cardId);

		var table = document.createElement("table");
		table.setAttribute("class", "w3-table w3-bordered w3-stripe");
		table.appendChild(thead);
		table.appendChild(tbody);

		return table;
	}

	this.addVariable = function(cardId, valueName, displayName, callbackCloseTableRow) {
		var close = document.createElement("a");
		close.appendChild(document.createTextNode("x"));
		close.setAttribute("class", "w3-closebtn");
		close.setAttribute("style", "font-size:15px; float:left;");
		close.setAttribute("href", "#");
		close.onclick = function () { callbackCloseTableRow(valueName, cardId); return false; }

		var td_close = document.createElement("td");
		td_close.setAttribute("style", "text-align:center; vertical-align:middle;");
		td_close.appendChild(close);

		var td_name = document.createElement("td");
		td_name.setAttribute("style", "text-align:center; vertical-align:middle;");
		td_name.appendChild(document.createTextNode(displayName));				

		var td_value = document.createElement("td");
		td_value.setAttribute("style", "text-align:center; vertical-align:middle; word-break:break-all;word-wrap:break-word");
		td_value.setAttribute("id", "variable_table_tr_value_" + valueName);
		td_value.appendChild(document.createTextNode("####"));

		var td_edit = document.createElement("td");
		td_edit.setAttribute("style", "text-align:center; vertical-align:middle;");
		var edit = document.createElement("input");
		edit.setAttribute("id", "variable_table_tr_edit_" + valueName);
		edit.setAttribute("style", "width:100px; padding:5px;");
		edit.setAttribute("class", "w3-input");
		//edit.setAttribute("required", "");
		var label = document.createElement("label");
		label.setAttribute("id", "variable_table_tr_edit_label_" + valueName);
		label.setAttribute("class", "w3-label");
		label.appendChild(document.createTextNode(" "));
		var group = document.createElement("div");
		group.setAttribute("class", "w3-group");
		group.setAttribute("style", "margin-top:15px; margin-bottom:0px;");
		group.appendChild(edit);
		group.appendChild(label);
		td_edit.appendChild(group);

		var td_button = document.createElement("td");
		td_button.setAttribute("style", "text-align:center; vertical-align:middle;");
		var button = document.createElement("Button");
		button.setAttribute("class", "w3-btn w3-green");
		button.appendChild(document.createTextNode("Schreiben"));
		button.onclick = function () { callbackWriteEvent(valueName); return false; }
		td_button.appendChild(button);

		var tr = document.createElement("tr");
		tr.setAttribute("id", "variable_table_tr_" + valueName);
		tr.appendChild(td_close);
		tr.appendChild(td_name);
		tr.appendChild(td_value);
		tr.appendChild(td_edit);
		tr.appendChild(td_button);

		var table = document.getElementById("table_cardid_" + cardId);
		table.appendChild(tr);
	}

	this.removeVariable = function(cardId, valueName) {	
		var table = document.getElementById("table_cardid_" + cardId);
		var tr = document.getElementById("variable_table_tr_" + valueName);
		table.removeChild(tr);
	}

	this.isTableEmpty = function(cardId) {
		var table = document.getElementById("table_cardid_" + cardId);		
		if (table.rows.length > 0) {
			return false;
		} else {
			return true;
		}
	}

	this.isTableFull = function(cardId) {
		var table = document.getElementById("table_cardid_" + cardId);
		if (table.rows.length < 4) {
			return false;
		} else {
			return true;
		}
	}
}

function TableHandler() {

	// **********************************************
	//  public

	this.setValue = function(valueName, value) {
		var td_value = document.getElementById("variable_table_tr_value_" + valueName);
		
		if (td_value != null) {
			td_value.innerHTML = value;
			return true;
		} 

		return false;
	}

	this.getValue = function(valueName) {
		var edit = document.getElementById("variable_table_tr_edit_" + valueName);
		if (edit == null) {
			return null;
		}

		var value = edit.value;
		edit.value = "";

		return value;
	}

	this.setStatus = function(valueName, status, color) {		
		var label = document.getElementById("variable_table_tr_edit_label_" + valueName);
		if (label != null) {
			label.innerHTML = status;
			if (color != null) {
				label.style.color = color;
			}
		}
	}
}

// ##############################################
//  Dashboard Panel
// ##############################################

function DashboardPanel() {

	// **********************************************
	//  public

	this.createCard = function(cardId, valueName, displayName, html_board_id, cardStyle, canvasStyle, callbackCloseCard) {
		var status = document.createElement("img");
		status.setAttribute("src", "./pics/icon_offline.png");
		status.setAttribute("style", "cursor:pointer; float:left; widtH:40px; margin-top:3px;");
		status.setAttribute("title", "initialisiere");
		status.setAttribute("id", "variable_dashboard_p_status_" + cardId);

		var title = document.createElement("h5");
		title.setAttribute("style", "margin-left:45px;");
		title.appendChild(document.createTextNode(displayName));

		var close = document.createElement("a");
		close.appendChild(document.createTextNode("X"));
		close.setAttribute("class", "w3-closebtn");
		close.setAttribute("href", "#");
		close.onclick = function () { callbackCloseCard(valueName, cardId); return false; }
		title.appendChild(close);

		var timestamp = document.createElement("p");
		timestamp.setAttribute("id", "variable_dashboard_p_timestamp_" + cardId);
		timestamp.setAttribute("style", "height:25px;");
		timestamp.appendChild(document.createTextNode("wird geladen..."));

		var canvas = document.createElement("canvas");
		canvas.setAttribute("id", "variable_div_board_canvas_" + cardId);
		canvas.setAttribute("style", "margin:auto; display:block;" + canvasStyle);
		
		var card = document.createElement("div");
		card.setAttribute("id", "variable_div_board_" + cardId);
		card.setAttribute("class", "w3-container w3-card-8 w3-margin");
		card.setAttribute("style", "float:left;" + cardStyle);
		card.appendChild(status);
		card.appendChild(title);
		card.appendChild(timestamp);
		card.appendChild(canvas);

		var board = document.getElementById(html_board_id);
		board.appendChild(card);

		// Canvas ID
		return "variable_div_board_canvas_" + cardId;
	}

	this.createTableCard = function(html_board_id, cardId) {
		var title = document.createElement("h5");
		title.appendChild(document.createTextNode("Control Table"));
		
		var tablePanel = new TablePanel(null);
		var table = tablePanel.createTable(cardId);

		var card = document.createElement("div");
		card.setAttribute("id", "variable_div_board_" + cardId);
		card.setAttribute("class", "w3-container w3-card-8 w3-margin");
		card.setAttribute("style", "width:660px; height:336px; float:left;");
		card.appendChild(title);
		card.appendChild(table);

		var board = document.getElementById(html_board_id);
		board.appendChild(card);
	}

	this.removeCard = function(html_board_id, cardId) {
		removeCardId(cardId);
		var board = document.getElementById(html_board_id);
		var card = document.getElementById("variable_div_board_" + cardId);
		board.removeChild(card);
	}

	// TODO find a better solution....

	var cardIds = [];

	this.generateNewCardId = function() {
		var i;
		for (i=0; i<64; i++) {
			if (cardIds[i] == null) {
				cardIds[i] = true;
				return i;
			}
		}
		return null;
	}

	function removeCardId(cardId) {
		cardIds[cardId] = null;
	} 

	// ................................
}


// ##############################################
//  Dashboard Handler
// ##############################################
function DashboardHandler() {

	// **********************************************
	//  public

	this.setStatus = function(cardId, status, icon) {
		var img = document.getElementById("variable_dashboard_p_status_" + cardId);	
		if (img != null) {
			if (img.title != status) {
				img.title = status;
			} 
			if (img.src != icon) {
				img.src = icon;
			}
		} 
	}

	this.setTimestamp = function(cardId, timestamp) {
		var p = document.getElementById("variable_dashboard_p_timestamp_" + cardId);
		if (p != null) {
			if (p.innerHTML != "<small>" + timestamp + "</small>") {
				p.innerHTML = "<small>" + timestamp + "</small>";
			}
		}
	}

}

// ##############################################
//  Dialog Panel
// ##############################################

function DialogPanel(callbackFromDialog) {

	// **********************************************
	//  public

	this.show = showChooseMenu;
	this.close = closeChooseMenu;

	// **********************************************
	//  private

	function showChooseMenu(html_dialog_id, valueName, displayObjs) {	
		// header
		var header = createHeader(html_dialog_id);
		var div_header = document.getElementById(html_dialog_id + "_header");
		div_header.appendChild(header);

		// content
		var ul = document.createElement("ul");
		ul.setAttribute("class", "w3-ul");
		
		var i;
		for (i=0; i<displayObjs.length; i++) {
			var displayObj = displayObjs[i];
			var li = createLi(valueName, displayObj);
			ul.appendChild(li);
		}

		var content = document.getElementById(html_dialog_id + "_content");
		content.appendChild(ul);

		// display dialog
		var modal = document.getElementById(html_dialog_id);
		modal.setAttribute("style", "display:table; position:absolute; overflow:scroll; z-index:1;");
	}

	function closeChooseMenu(html_dialog_id) {
		// clean header
		var header = document.getElementById(html_dialog_id + "_header");
		while (header.hasChildNodes()) {
		    header.removeChild(header.firstChild);
		}
		
		// clean content
		var content = document.getElementById(html_dialog_id + "_content");
		while (content.hasChildNodes()) {
		    content.removeChild(content.firstChild);
		}
		
		// undisplay dialog
		var modal = document.getElementById(html_dialog_id);
		modal.setAttribute("style", "");
	} 

	function createHeader(html_dialog_id) {
		var close_a = document.createElement("a");
		close_a.setAttribute("href", "#");
		close_a.setAttribute("class", "w3-closebtn");
		close_a.onclick = function () { closeChooseMenu(html_dialog_id); return false; }
		close_a.appendChild(document.createTextNode("X"));

		var title = document.createElement("h3");
		title.appendChild(document.createTextNode("Wähle ein Display-Instrument"));

		var header = document.createElement("header");
		header.setAttribute("class", "w3-container w3-green");
		header.appendChild(close_a);
		header.appendChild(title);

		return header;
	}

	function createLi(valueName, displayObj) {
		var icon = document.createElement("img");
		icon.setAttribute("style", "width:85px; height:85px;");
		icon.setAttribute("src", displayObj.icon);

		var a = document.createElement("a");
		a.setAttribute("style", "text-decoration: none;");
		a.setAttribute("href", "#");
		a.setAttribute("title", displayObj.description);
		a.onclick = function () { callbackFromDialog(valueName, displayObj); return false; }
		a.appendChild(icon);

		var li = document.createElement("li");
		li.setAttribute("style", "float:left;");
		li.appendChild(a);

		return li;
	}
}