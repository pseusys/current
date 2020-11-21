import {Component, Input, OnInit} from '@angular/core';
import {Action, Broker, ProviderService} from '../provider.service';
import { v4 as uuidv4 } from 'uuid';
import {Router} from '@angular/router';

@Component({
  selector: 'app-actions',
  templateUrl: './actions.component.html'
})
export class ActionsComponent implements OnInit {
  @Input() current!: Action;
  user: Broker = new Broker();
  id = 'y' + uuidv4();

  constructor(private router: Router, private provider: ProviderService) { }

  ngOnInit(): void { this.provider.get_user().then((data) => { this.user = data; }); }

  on_edit_action(): void {
    this.provider.edit_action(this.current).then(() => {
      this.router.navigate(['/actions']);
    });
  }

  on_delete_action(): void {
    this.provider.delete_action(this.current.name).then(() => {
      this.router.navigate(['/actions']);
    });
  }
}
