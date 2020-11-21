import { Component, Input, OnInit, TemplateRef } from '@angular/core';
import { Router } from '@angular/router';
import {Action, Broker, ProviderService, Settings} from '../provider.service';

@Component({
  selector: 'app-main',
  templateUrl: './main.component.html',
  styleUrls: ['./main.component.css']
})
export class MainComponent implements OnInit {
  href: string;
  @Input() baseBody!: TemplateRef<any>;
  leftHeaderButton = '';
  rightHeaderButton = '';

  brokers: Broker[] = [];
  actions: Action[] = [];

  user: Broker = new Broker();
  newAction: Action = new Action();
  settings: Settings = new Settings(new Date(), new Date(), 0);

  constructor(private router: Router, private provider: ProviderService) { this.href = router.url.substring(1); }

  ngOnInit(): void {
    this.provider.get_user().then((data) => { this.user = data; });
    if (!!this.href) {
      this.leftHeaderButton = '🠔';
      this.rightHeaderButton = '+';
      if (this.href === 'brokers') { this.provider.get_brokers().then((data) => { this.brokers = data; }); }
      else if (this.href === 'actions') { this.provider.get_actions().then((data) => { this.actions = data; }); }
    } else {
      this.leftHeaderButton = '+';
      this.rightHeaderButton = '■';
      this.provider.get_settings().then((data) => { this.settings = data; });
    }
  }

  on_login(): void { this.router.navigate(['/login']); }

  on_back(): void { this.router.navigate(['/']); }

  on_add_action(): void {
    this.provider.create_action(this.newAction).then(() => {
      this.newAction = new Action();
      this.router.navigate(['/actions']);
    });
  }

  on_edit_user(): void { this.provider.edit_user(this.user); }

  on_delete_user(): void {
    this.provider.delete_user().then(() => {
      this.router.navigate(['/login']);
    });
  }

  on_brokers(): void { this.router.navigate(['/brokers']); }

  on_actions(): void { this.router.navigate(['/actions']); }

  set_settings(): void {
    this.provider.set_settings(this.settings).then(() => {
      this.router.navigate(['/']);
    });
  };
}
