import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { CookieService } from 'ngx-cookie-service';


export class Broker {
  public name: string;
  public money: number;
  public isAdmin: boolean;

  constructor(name?: string, money?: number, isAdmin?: boolean) {
    if (name !== undefined) { this.name = name; }
    else { this.name = ''; }
    if (money !== undefined) { this.money = money; }
    else { this.money = 0; }
    if (isAdmin !== undefined) { this.isAdmin = isAdmin; }
    else { this.isAdmin = false; }
  }

  public admin(): boolean { return (this.name !== '') && this.isAdmin; }

  public member(): boolean { return this.name !== ''; }
}


enum Laws {
  NORMAL = 'normal',
  EQUAL = 'equal'
}

export class Action {
  public name: string;
  public law: Laws;
  public maximum: number;
  public quantity: number;
  public cost: number;

  constructor() {
    this.name = '';
    this.law = Laws.EQUAL;
    this.maximum = 0;
    this.quantity = 0;
    this.cost = 0;
  }
}


export class Settings {
  public tradeStart: Date;
  public tradeEnd: Date;
  public interval: number;

  constructor(startDate: Date, endDate: Date, interval: number) {
    this.tradeStart = startDate;
    this.tradeEnd = endDate;
    this.interval = interval;
  }
}


@Injectable({
  providedIn: 'root'
})
export class ProviderService {
  private headers: HttpHeaders;
  private cors = 'http://localhost:8081';
  private key = 'username';

  constructor(private http: HttpClient, private cookieService: CookieService) {
    this.headers = new HttpHeaders();
    this.headers.append('Content-Type', 'application/json');
    this.headers.append('projectid', 'client-web-5');
  }

  async get_brokers(): Promise<Broker[]> {
    try {
      return await this.http.get<Broker[]>(this.cors + '/users', { headers: this.headers }).toPromise().then((data) => {
        const brokers = [];
        for (const broker of data) { brokers.push(new Broker(broker.name, broker.money, broker.isAdmin)); }
        return brokers;
      });
    } catch (e) {
      console.log('Error fetching brokers: ', e);
      alert(e.error);
      return new Promise<Broker[]>(() => []);
    }
  }

  async get_actions(): Promise<Action[]> {
    try {
      return await this.http.get<Action[]>(this.cors + '/actions', { headers: this.headers }).toPromise();
    } catch (e) {
      console.log('Error fetching actions: ', e);
      alert(e.error);
      return new Promise<Action[]>(() => []);
    }
  }


  async create_user(username: string, password: string): Promise<Broker> {
    const params = (new HttpParams()).set('username', username).set('password', password);
    try {
      return await this.http.post<Broker>(this.cors + '/signin', params).toPromise().then((data) => {
        this.cookieService.set(this.key, data.name);
        return new Broker(data.name, data.money, data.isAdmin);
      });
    } catch (e) {
      console.log('Error creating user:', e);
      alert(e.error);
      return new Promise<Broker>(() => new Broker());
    }
  }

  async login(username: string, password: string): Promise<Broker> {
    const params = (new HttpParams()).set('username', username).set('password', password);
    try {
      return await this.http.post<Broker>(this.cors + '/login', params).toPromise().then((data) => {
        this.cookieService.set(this.key, data.name);
        return new Broker(data.name, data.money, data.isAdmin);
      });
    } catch (e) {
      console.log('Error logging in user: ', e);
      alert(e.error);
      return new Promise<Broker>(() => new Broker());
    }
  }

  async get_user(): Promise<Broker> {
    const params = (new HttpParams()).set('user', this.cookieService.get(this.key));
    try {
      return await this.http.get<Broker>(this.cors + '/user', { headers: this.headers, params }).toPromise().then((data) => {
        return new Broker(data.name, data.money, data.isAdmin);
      });
    } catch (e) { return new Promise<Broker>(() => new Broker()); }
  }

  async edit_user(user: Broker): Promise<Broker> {
    const params = (new HttpParams()).set('user', JSON.stringify(user));
    try {
      return await this.http.post<Broker>(this.cors + '/user', params).toPromise().then((data) => {
        return new Broker(data.name, data.money, data.isAdmin);
      });
    } catch (e) {
      console.log('Error editing user: ', e);
      alert(e.error);
      return new Promise<Broker>(() => new Broker());
    }
  }

  async delete_user(): Promise<void> {
    const params = (new HttpParams()).set('user', this.cookieService.get(this.key));
    try {
      return await this.http.delete<void>(this.cors + '/signin', { headers: this.headers, params }).toPromise().then(() => {
        this.cookieService.set(this.key, '');
      });
    } catch (e) {
      console.log('Error deleting user: ', e);
      alert(e.error);
      return new Promise<void>(() => { });
    }
  }

  async logoff(): Promise<void> {
    try {
      return await this.http.delete<void>(this.cors + '/login', { headers: this.headers }).toPromise().then(() => {
        this.cookieService.set(this.key, '');
      });
    } catch (e) {
      console.log('Error logging off user: ', e);
      alert(e.error);
      return new Promise<void>(() => { });
    }
  }


  async create_action(action: Action): Promise<Action> {
    const params = (new HttpParams()).set('action', JSON.stringify(action));
    try {
      return await this.http.post<Action>(this.cors + '/action', params).toPromise();
    } catch (e) {
      console.log('Error creating action: ', e);
      alert(e.error);
      return new Promise<Action>(() => new Action());
    }
  }

  async edit_action(action: Action): Promise<Action> {
    const params = (new HttpParams()).set('action', JSON.stringify(action));
    try {
      return await this.http.put<Action>(this.cors + '/action', params).toPromise();
    } catch (e) {
      console.log('Error editing action: ', e);
      alert(e.error);
      return new Promise<Action>(() => new Action());
    }
  }

  async delete_action(name: string): Promise<void> {
    const params = (new HttpParams()).set('name', name);
    try {
      return await this.http.delete<void>(this.cors + '/action', { headers: this.headers, params }).toPromise();
    } catch (e) {
      console.log('Error editing action: ', e);
      alert(e.error);
      return new Promise<void>(() => { });
    }
  }


  async get_settings(): Promise<Settings> {
    try {
      return await this.http.get<Settings>(this.cors + '/settings', { headers: this.headers }).toPromise().then((data) => {
        return new Settings(data.tradeStart, data.tradeEnd, data.interval);
      });
    } catch (e) { return new Promise<Settings>(() => new Settings(new Date(), new Date(), 0)); }
  }

  async set_settings(settings: Settings): Promise<Settings> {
    const params = (new HttpParams()).set('settings', JSON.stringify(settings));
    try {
      return await this.http.put<Settings>(this.cors + '/settings', params).toPromise();
    } catch (e) {
      console.log('Error editing settings: ', e);
      alert(e.error);
      return new Promise<Settings>(() => new Settings(new Date(), new Date(), 0));
    }
  }
}
