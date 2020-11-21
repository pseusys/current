import './App.css';
import './MoneySpan.css';
import React, { Component } from "react";
import { set_listener } from "./Socketizer";

export default class MoneySpan extends Component {
    constructor(props) {
        super(props);
        this.state = { preValue: -1, postValue: -1 };
        this.callback = this.callback.bind(this);
        set_listener(props.id, this.callback);
    }

    callback(ongoing, user) {
        if (ongoing) {
            if (this.state.preValue === -1) this.setState({ preValue: user.money });
            this.setState({ postValue: user.money });
        }
        else this.setState({ preValue: user.money, postValue: -1 });
    }

    render() {
        return (
            <div id="buttonier" className="row bright">
                <div className="span col-6">
                    <span className="titel">До подключения к торгам: { this.state.preValue }</span>
                </div>
                <div className="span col-6">
                    <span className="titel">{ this.state.postValue !== -1 ? ("На данный момент: " + this.state.postValue) : "" }</span>
                </div>
            </div>
        );
    }
}
