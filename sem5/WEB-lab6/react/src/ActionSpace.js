import './App.css';
import './ActionSpace.css';
import React, {Component} from "react";
import { buy, sell, set_listener } from "./Socketizer";
import { v4 as uuidv4 } from 'uuid';

export default class MoneySpan extends Component {
    constructor(props) {
        super(props);
        this.state = { stock: props.stock, actions: [], empty: true, loaded: false };
        this.callback = this.callback.bind(this);
        set_listener(props.id, this.callback);
    }

    callback(ongoing, user, actions) {
        this.setState({ empty: false });
        if (ongoing) {
            this.user = user;
            this.actions = actions;
        } else {
            if (this.state.stock) this.setState({ empty: true });
            else {
                this.user = user;
                this.actions = actions;
            }
        }
        this.setState({ loaded: true });
    }

    get_action_info(name) {
        const action = this.actions.find((e) => { return e.name === name; });
        const quantity = this.state.stock ?
            action.stocked : this.user.actions.find((e) => { return e.name === name; }).quantity;
        const id = "s" + uuidv4();
        return (
            <div className="row" key={ name }>
                <div className="col-3">{ name }</div>
                <div className="col-2">{ quantity }</div>
                <div className="col-2">{ action.cost }</div>
                <div className="col-2 my-auto">
                    <button className="in-button button-small" onClick={ () => {
                        const number = Number.parseInt(document.getElementById(id).value);
                        if (this.state.stock) buy(name, number);
                        else sell(name, number);
                    } }>{ this.state.stock ? "Купить" : "Продать" }</button>
                </div>
                <div className="col-3">
                    <form>
                        <div className="align-items-center">
                            <div className="col-auto">
                                <label className="sr-only" htmlFor="inlineFormInput">Name</label>
                                <input type="number" className="intruder" id={ id } defaultValue={0}
                                       placeholder="Кооличество акций"/>
                            </div>
                        </div>
                    </form>
                </div>
            </div>
        );
    }

    get_body(actions) {
        let children = [];
        for (const action of actions) children.push(this.get_action_info(action.name));
        return (
            <div className="col scrollable">
                <div className="text-center">{ this.state.stock ? "В продаже:" : "У меня:" }</div>
                <div className="row">
                    <div className="col-3">Название</div>
                    <div className="col-2">Количество</div>
                    <div className="col-2">Стоимость</div>
                </div>
                { children }
            </div>
        );
    }

    get_filler() {
        return (<div className="col-4 offset-4 my-auto">Торги не идут!</div>);
    }

    render() {
        const base_class = "semi-body row bright";
        let body;
        if (!this.state.loaded) body = (<div className="col-4 offset-4 my-auto">Информация загружается...</div>);
        else body = this.get_body(this.state.stock ? this.actions : this.user.actions);
        return (
            <div className={base_class + (this.state.empty ? " text-center" : "")}>
                { this.state.empty ? this.get_filler() : body }
            </div>
        );
    }
}
