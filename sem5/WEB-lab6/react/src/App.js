import './App.css';
import React, {Component} from "react";
import MoneySpan from "./MoneySpan";
import ActionSpace from "./ActionSpace";

export default class App extends Component {
    render() {
        return (
            <div className="container-fluid" id="basic">
                <header id="header" className="row">
                    <div className="col-2 my-auto">
                        <button type="button" className="in-button" onClick={() => {
                            window.location.href = "localhost:4200/";
                        }}>🠔</button>
                    </div>
                    <div className="col-8 bright">
                        <h6 id="title">Биржа</h6>
                    </div>
                </header>
                <MoneySpan id="money_span"/>
                <ActionSpace id="action_space_buy" stock={ true }/>
                <ActionSpace id="action_space_sell" stock={ false }/>
            </div>
        );
    }
}
